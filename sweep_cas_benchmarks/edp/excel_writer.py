import os
import sys
from pathlib import Path
import xlsxwriter
import argparse
from datetime import datetime
import time

def get_sheet_and_format(workbook, name, tab_color, font_size, num_format, font = 'Calibri'):
    worksheet = workbook.add_worksheet(name)
    worksheet.set_tab_color(tab_color)
    format = workbook.add_format({'num_format': num_format, 'font_size': font_size, 'font_name': font})
    return worksheet, format

def import_file(workbook, file, name):
    if not file or not Path(file).is_file():
         print(' WARNING: {} file does not exist.'.format(name))
         return

    print('     importing {}'.format(name))
    with open(file) as f:
         sheet, fmt = get_sheet_and_format(workbook, name, '#001400', 10, '#,##0')
         for row_index, line in enumerate(f, 0):
            for col_index, col in enumerate(line.split('\t'), 0):
                sheet.write(row_index, col_index, col, fmt)

    print('     imported {}'.format(name))

def import_summary(workbook, file, name, tab_color, font_size, format, font_name):
    if not file or not Path(file).is_file():
        return

    sheet, format = get_sheet_and_format(workbook, name, tab_color, font_size, format, font_name)
    row_length = 0
    with open(file) as f:
        for row_index, line in enumerate(f, 0):
            for col_index, value in enumerate(line.split(','), 0):
                sheet.write(row_index, col_index, value, format)
            row_length += 1
    sheet.set_column(0, 0, 40)
    sheet.set_column(1, row_length, 22.36)

def import_view(workbook, args, view):
    print('     importing {} view...'.format(view))

    summary_csv         = '{}/__edp_{}_view_summary.csv'.format(args.temp_dir, view)
    per_txn_summary_csv = '{}/__edp_{}_view_summary.per_txn.csv'.format(args.temp_dir, view)
    details_csv         = '{}/__edp_{}_view_details.csv'.format(args.temp_dir, view)

    # ------Details-------:
    num_samples = 0
    headers = []
    row_length = 0
    if details_csv and Path(details_csv).is_file():
        sheet, format = get_sheet_and_format(workbook, 'details {} view'.format(view), '#630000', 8, '#,##0.0000')
        first_col_format = workbook.add_format({'num_format': '#,##0', 'font_size': 8, 'font_name': 'Calibri'})
        with open(details_csv) as f:
            for row_index, line in enumerate(f, 0):
                row = line.split(',')
                sheet.write(row_index, 0, row[0], first_col_format)
                if row_index == 0:
                    headers = row

                if row[1] != 'timestamp' and not args.timestamp_in_chart:
                    row[1] = datetime.strptime(row[1], '%m/%d/%Y %H:%M:%S.%f')

                for col_index in range(1, len(row)):
                    # If we don't do this, then file size balloons as detail data is large!
                    # Also NaN causes trouble with Excel trying to read charts
                    if row[col_index] == '' or row[col_index] == 'NaN':
                        continue
                    sheet.write(row_index, col_index, row[col_index], format)
                row_length = len(row)
                num_samples += 1
        sheet.set_column(20, row_length, 12.45)

    # ------Sumarry-------:
    import_summary(workbook, summary_csv, '{} view'.format(view), '#006300', 10, '#,##0.0000', 'Courier New')
    if args.tps:
        import_summary(workbook, per_txn_summary_csv, '{} view (per-txn)'.format(view), '#006300', 10, '#,##0.0000', 'Courier New')

    # ------Chart---------:
    if args.format:
        import_chart(workbook, args, view, headers, num_samples)

def import_chart(workbook, args, view, headers, num_samples):
    if not args.format or not Path(args.format).is_file():
        return

    chart_sheet, chart_format = get_sheet_and_format(workbook, 'chart {} view'.format(view), '#000063', 10, None, 'Calibri')

    print('        plotting charts for {} view: '.format(view), end='')

    data_sheet_name = '\'details {} view\''.format(view)

    cat_col = 'timestamp' if args.timestamp_in_chart else '#sample'
    for col_index, header in enumerate(headers, 0):
        if header == cat_col:
            categories = [data_sheet_name, 1, col_index, num_samples, col_index]

    row = 1
    x_offset = 10
    col = 0

    num_charts = 0
    start_time = time.time()
    with open(args.format) as f:
        for metric in f:
            metric = metric.strip()
            if metric == '':
                row += 18
                col = 0
                x_offset = 10
                continue

            num_series = 0
            chart_not_created = True
            for col_index, header in enumerate(headers, 0):
                if metric == header or (metric + ' ') in header:
                    values = [data_sheet_name, 1, col_index, num_samples, col_index]

                    if chart_not_created:
                        chart = workbook.add_chart({'type': 'scatter', 'subtype': 'smooth_with_markers'})
                        chart_not_created = False

                    chart.add_series({
                        'name':       '{}'.format(header),
                        'categories': categories,
                        'values':     values,
                    })
            # Chart w/o series results in error when writing to excel (this is by design of the library)
            if chart_not_created:
                continue

            chart.set_legend({'position': 'bottom'})
            chart.set_title({'name': metric, 'name_font': {'size': 11}})
            chart.set_size({'width': 408, 'height': 343.68, 'x_offset': x_offset})
            chart_sheet.insert_chart(row, col, chart)
            col += 6
            x_offset += 35
            num_charts += 1
            print('+', end='')
        print('\n        {} charts plotted in {} seconds. '.format(num_charts, time.time() - start_time))

def sort_sheets(workbook):
    sheets = ['Configuration', 'system view', 'socket view', 'core view', 'thread view',
              'system view (per-txn)', 'socket view (per-txn)', 'core view (per-txn)', 'thread view (per-txn)',
              'chart system view', 'chart socket view', 'chart core view', 'chart thread view',
              'details system view', 'details socket view', 'details core view', 'details thread view',
              'emonV', 'emonM', 'Disk Stat', 'Network Stat', 'dimdecode']
    available_sheets = []
    sheet_dict = {}
    for index, x in enumerate(workbook.worksheets_objs, 0):
        available_sheets.append(x.name)
        sheet_dict[x.name] = index

    sheets = [sheet for sheet in sheets if sheet in available_sheets]
    order = []
    for sheet in sheets:
        order.append(sheet_dict[sheet])

    workbook.worksheets_objs = [workbook.worksheets_objs[i] for i in order]

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Arguments for EDP Excel Writer')
    parser.add_argument('--output', action='store', help='Excel output file name]')
    parser.add_argument('--temp-dir', action='store', default='.', help='Temp directory for intermediate CSV file]')
    parser.add_argument('--format', action='store', default=None, help='Chart format file [optional]')
    parser.add_argument('--config', action='store', default=None, help='Configuration file [optional]')
    parser.add_argument('--disk-stat', action='store', default=None, help='DiskStat file [optional]')
    parser.add_argument('--dmidecode', action='store', default=None, help='dmidecode file [optional]')
    parser.add_argument('--emonv', action='store', default=None, help='emonv file [optional]')
    parser.add_argument('--emonm', action='store', default=None, help='emonm file [optional]')
    parser.add_argument('--network-stat', action='store', default=None, help='Nework stat file [optional]')
    parser.add_argument('--tps', action='store', default=None, help='Transactions per second')
    parser.add_argument('--core-view', action='store_true', help='Include core view')
    parser.add_argument('--socket-view', action='store_true', help='Include socket view')
    parser.add_argument('--thread-view', action='store_true', help='Include thread view')
    parser.add_argument('--timestamp-in-chart', action='store_true', help='Timestamp of the chart')

    args = parser.parse_args()

    if not args.output:
        print('Required argument output file name is missing!')
        sys.exit(1)

    if '.xlsx' not in args.output:
       args.output += '.xlsx'

    workbook = xlsxwriter.Workbook(args.output, {'strings_to_numbers': True, 'constant_memory': True})

    import_file(workbook, args.config, 'Configuration')
    import_file(workbook, args.emonm, 'emonM')
    import_file(workbook, args.emonv, 'emonV')
    import_file(workbook, args.disk_stat, 'Disk Stat')
    import_file(workbook, args.network_stat, 'Network Stat')
    import_file(workbook, args.dmidecode, 'dimdecode')

    import_view(workbook, args, 'system')
    if args.socket_view:
        import_view(workbook, args, 'socket')
    if args.core_view:
        import_view(workbook, args, 'core')
    if args.thread_view:
        import_view(workbook, args, 'thread')

    sort_sheets(workbook)

    start_time = time.time()
    workbook.close()
    print('Wrote Excel file in {}'.format(time.time() - start_time))

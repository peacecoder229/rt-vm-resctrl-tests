
$perf_name_slots = "TOPDOWN.SLOTS:perf_metrics"
$perf_names_toplev = ["PERF_METRICS.RETIRING", "PERF_METRICS.BAD_SPECULATION","PERF_METRICS.FRONTEND_BOUND","PERF_METRICS.BACKEND_BOUND"]

def AdjustPerfmetrics(sample)
	event_dict = sample.events
	if event_dict.has_key?($perf_name_slots) && $perf_names_toplev.all? { |n| event_dict.has_key? n }
		denom = event_dict.values_at(*$perf_names_toplev).map {|event| event.fields}.transpose.map{|arr| arr.sum}
		slots_val = event_dict[$perf_name_slots].fields
		event_dict.each do |key, value|
			if key.start_with?("PERF_METRICS.")
				value.fields = denom.each_with_index.collect {|local_denom, index|
					local_denom == 0 ? 0 : (slots_val[index]* value.fields[index] / local_denom)
				}
			end
		end
	end
end
require 'optparse'

PREPROC_VERSION = "4.23"

class Options
  
  def initialize
   parse_options
  end
	
  # parse the options from ARGV
  def parse_options
    opts = OptionParser.new do |opts|
      opts.banner = "Usage: SplitByCoreType.rb [options]\n  ver: #{PREPROC_VERSION}"
      opts.program_name = "SplitByCoreType"
      opts.version = PREPROC_VERSION
      opts.separator "\nRequired options:"
      opts.on("-i", "--input <emon.dat>", "input file: emon dat") { |file| @input_file = file }
      opts.on("-o", "--output <output.dat>", "output file name") { |file| @output_file = file }
      opts.separator "\nOptional options:"
      opts.on("-v", "--verbose", "verbose output to console (not to the output file)") { @verbose = true; }
      opts.separator "\nMisc options:"
      opts.on("-h", "--help", "Show this message") { $stdout.puts opts; exit }
      opts.on("--version", "Show version") { $stdout.puts opts.ver; exit}
    end
	
    if ARGV.size==0
      $stdout.puts opts
      exit
    else
      opts.parse!(ARGV)
    end

  end

  # an alternative to define attribute reader for every possible attribute
  def method_missing(key)
    eval("@"+key.to_s) if instance_variables.include?("@#{key.to_s}".to_sym) || instance_variables.include?("@#{key.to_s}")
  end
end

class CpuInfo
	def initialize(os_proc, package, core, thread, natural_index, core_type)
	  @os_proc, @package, @core, @thread, @natural_index, @core_type = os_proc, package, core, thread, natural_index, core_type
	end
	
	def self.from_line(line, natural_index)
		fields = /^\s*(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\w+)/.match(line)
		if fields
			cpu, processor, core, thread = *fields[1..-2].collect{|x| x.to_i}
			core_type = fields[-1].downcase
			return CpuInfo.new(cpu, processor, core, thread, natural_index, core_type)
		else
			return nil
		end
	end
	
	def to_line(delimiter)
		return "#{delimiter}#{@os_proc}#{delimiter}#{@package}#{delimiter}#{@core}#{delimiter}#{@thread}"
	end
	
	attr_reader :os_proc, :package, :core, :thread, :natural_index, :core_type
end

def is_tsc_or_event(value)
	if /^\d[\d,]*$/.match(value)
		return true
	else
		return false
	end
end

class TopologyMorpher
	
	def initialize(options)
		@cpus = readTopology(options.input_file)
		@core_primary_cpu = Hash[@cpus.uniq{|cpu| cpu.core}.collect {|cpu| [cpu.core, cpu]}] # cpus is already sorted by natural_index
	end

	# parse the emon.dat data to get CPU toptology information
	def readTopology(file)
		cpus = []
		read_index = 0
		
		if ! file.nil?
			header_found = false
			File.open(file) do |f|
				f.each_line do |line|
					if line.include? "OS Processor"
						header_found=true
						next
					end
			
					if header_found
						cpu = CpuInfo.from_line(line, read_index)
						if cpu
							read_index += 1
							cpus << cpu
						end
					end
			
					if line.include? "Version Info" 
						break
					end
				end
			end
		else
			$stderr.puts "ERROR: Input file not provided"
		end
		
		return cpus
	end
	
	
	def as_reduced_events(line, retain_cpus, retain_cores)
	
		guaranteed_length = 2 # valid data has at least 3 fields: event name, tsc, one or more per-cpu data
		tsc_offset = 1
		num_cpus = @cpus.length()
		num_cores = @core_primary_cpu.length()
		
		columns = line.split(/\s+/)
		valid_event = true
		if columns.length() > guaranteed_length && is_tsc_or_event(columns[tsc_offset])
			if columns.length() == num_cpus + guaranteed_length
				return _as_reduced_events(columns, guaranteed_length, retain_cpus)
			elsif columns[0].downcase.start_with?("unc_")
				if columns.length() == num_cores + guaranteed_length
					return _as_reduced_events(columns, guaranteed_length, retain_cores)
				else
					return columns
				end
			end
		end
		
		return [line]
	end
	
	def _as_reduced_events(columns, known_offset, retention_offsets)
		
		ret_columns = columns.first(known_offset)
		valid = true
		retention_offsets.each do |retention_offset|
			val = columns[known_offset + retention_offset]
			if is_tsc_or_event(val)
				ret_columns << val
			else
				valid = false
				ret_columns = nil
				break
			end
		end
		
		return ret_columns
	end
	
	def writeTopology(input_file, output_file, cpus)
		# Ensure CPUs are unique and ordered
		ordered_cpus = cpus.uniq.sort{|a,b| a.natural_index <=> b.natural_index}
		retain_cpus = ordered_cpus.map(&:natural_index)
		retain_cores = (ordered_cpus.map(&:core).uniq).map{|core_idx| @core_primary_cpu[core_idx].natural_index}
		
		# Clone input header until topology
		if (! input_file.nil?) && (! output_file.nil?)
			header_found = false
			header_phase = true
			topology_written = false
			ofile = File.open(output_file, 'w')
			
			File.open(input_file) do |f|
				f.each_line do |line|
					if header_phase
						if (! header_found)
							ofile.write(line)
							if line.include? "OS Processor"
								header_found=true
							end
							next
						end
				
						if header_found
							cpu = CpuInfo.from_line(line, 0)
							if ! cpu
								ofile.write(line)
							elsif ! topology_written
								ordered_cpus.each{|cpu| ofile.puts(cpu.to_line("\t\t"))}
								topology_written = true
							end
						end
				
						if line.include? "Version Info" 
							header_phase = false
						end
					else
						reduced_row_array = as_reduced_events(line, retain_cpus, retain_cores)
						if reduced_row_array
							ofile.puts(reduced_row_array.join("\t"))
						else
							#ofile.write(line)
						end
					end
				end
			end
			
			ofile.close()
		else
			$stderr.puts "ERROR: Input file not provided"
		end
	end
	
	attr_reader :cpus
end


def main
	puts "\nSplitByCoreType version #{PREPROC_VERSION}; Ruby platform is #{RUBY_ENGINE} #{RUBY_VERSION}"

	options = Options.new
	morpher = TopologyMorpher.new(options)
	#puts "#{morpher.cpus}"
	morpher.cpus.map {|cpu| cpu.core_type}.uniq.each {|core_type|
		morpher.writeTopology(options.input_file, "#{options.output_file}_#{core_type}", morpher.cpus.select {|cpu| cpu.core_type == core_type})
	}

end

main
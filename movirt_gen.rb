
require 'erb'

filenames = ARGV
filenames.each do |fn|
	open(fn) do |src|
		open(fn.gsub(/\.erb/, ''), 'w') do |dst|
			dst.write(ERB.new(src.read).result)
		end
	end
end

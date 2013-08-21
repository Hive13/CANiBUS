#!/usr/bin/ruby
# Simple script to convert VehicleSpy CSV files to CANiBUS JSON format
#
# craig@hive13.org

require "csv"

puts "["
first = true
CSV.foreach(ARGV[0]) do |row|
  puts "," if not first
  #puts row.inspect
  print "{ "
  print "\"SeqNo\":#{row[0].to_i}"
  print ", \"AbsTime\": \"#{row[1]}\""
  print ", \"RelTime\": \"#{row[2]}\""
  print ", \"Status\": \"#{row[3]}\""
  print ", \"Error\": \"#{row[4]}\""
  print ", \"Transmit\": \"#{row[5]}\""
  print ", \"Desc\": \"#{row[6]}\""
  print ", \"Network\": \"#{row[7]}\""
  print ", \"Node\": \"#{row[8]}\""
  print ", \"ArbID\": \"#{row[9]}\""
  print ", \"Remote\": #{row[10] == "F" ? "false" : "true"}"
  print ", \"Extended\": #{row[11] == "F" ? "false" : "true"}"
  print ", \"B1\": #{row[12] ? row[12].hex.to_i : 0}"
  print ", \"B2\": #{row[13] ? row[13].hex.to_i : 0}"
  print ", \"B3\": #{row[14] ? row[14].hex.to_i : 0}"
  print ", \"B4\": #{row[15] ? row[15].hex.to_i : 0}"
  print ", \"B5\": #{row[16] ? row[16].hex.to_i : 0}"
  print ", \"B6\": #{row[17] ? row[17].hex.to_i : 0}"
  print ", \"B7\": #{row[18] ? row[18].hex.to_i : 0}"
  print ", \"B8\": #{row[19] ? row[19].hex.to_i : 0}"
  print ", \"Value\": \"#{row[20]}\""
  print ", \"Trigger\": \"#{row[21]}\""
  print ", \"Signals\": \"#{row[22]}\""
  first = false
  print "}"
end
puts ""
puts "]"

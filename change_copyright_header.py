import sys, tempfile, os

if len(sys.argv) != 3:
	print "Usage: " + sys.argv[0] + " header_file source_file"
	exit(1)

# save the temporary file name
temp_file_name = None
with tempfile.NamedTemporaryFile(dir=".", delete = False) as f:
	temp_file_name = f.name
	# add the new header to the file
	with open(sys.argv[1], "rb") as header:
		for line in header:
			f.write(line)

	# read source file, skipping any previous header
	with open(sys.argv[2], "r+") as orig:
		is_original_header = True
		for line in orig:
			if is_original_header:
				# Use a simple algorithm to find out when the original header finishes;
				# look for '#' as the first char, that means the header has finished
				if line[0] == "#":
					f.write(line)
					is_original_header = False
			else:
				f.write(line)
	
try:
	os.rename(temp_file_name, sys.argv[2])
except OSError:
	print "Couldn't write to " + sys.argv[2] + "; maybe they are on different filesystems or " + sys.argv[2] + " already exists"





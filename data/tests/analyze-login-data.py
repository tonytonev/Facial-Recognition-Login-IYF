datafile = "/Users/Tony/Dropbox/Code/InYourFace (dropbox)/Trunk/data/test-results/loginresults.txt"

# Read data
data = open(datafile).read().split('\n')[2:-1]

# Parse into easy-to-use type
data = [a.split('\t') for a in data]
data = [a[:3] + a[4:] for a in data if a[3] == ''] + [a for a in data if a[3] != '']
data = [(a[1], a[-1].split('/')[-2], a[-1].split('/')[-1], a[3] == 'True', a[4] == 'True') for a in data]
data = list(set(data))

users = {}

# Sort data by user in users dict
for d in data:
    if users.has_key(d[0]):
        users[d[0]].append(d)
    else:
        users[d[0]] = [d]

print "Name\tMatch Acc\tNonMatch Acc\tOverall Acc"

for u, v in users.items():
    matches = [a for a in v if a[-2]]
    nonmatches = [a for a in v if not a[-2]]

    good_matches = [a for a in matches if a[-1]]
    good_nonmatches = [a for a in nonmatches if not a[-1]]

    print "%s\t%f%%\t%f%%\t%f%%" % (u, 100*float(len(good_matches))/len(matches), 100*float(len(good_nonmatches))/len(nonmatches), 100*float(len(good_matches) + len(good_nonmatches))/len(v))

matches = [a for a in data if a[-2]]
nonmatches = [a for a in data if not a[-2]]

good_matches = [a for a in matches if a[-1]]
good_nonmatches = [a for a in nonmatches if not a[-1]]

print "\n%s\t%f%%\t%f%%\t%f%%" % ("Overall", 100*float(len(good_matches))/len(matches), 100*float(len(good_nonmatches))/len(nonmatches), 100*(float(len(good_matches)) + len(good_nonmatches))/len(data))

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

print "User\tPrecision\tRecall\tFallout"

# Calculate for each user
for u, d in users.items():
    all_t = set(d)
    matches = set([a for a in d if a[-2]])
    accepted = set([a for a in d if a[-1]])

    nonmatches = all_t - matches
    accepted_matches = matches.intersection(accepted)
    accepted_nonmatches = nonmatches.intersection(accepted)

    precision = float(len(accepted_matches))/len(accepted)
    recall = float(len(accepted_matches))/len(matches)
    fallout = float(len(accepted_nonmatches))/len(nonmatches)

    print "%s\t%.2f%%\t%.2f%%\t%.2f%%" % (u, 100*precision, 100*recall,100*fallout)

# Calculate overall
all_t = set(data)
matches = set([a for a in data if a[-2]])
accepted = set([a for a in data if a[-1]])

nonmatches = all_t - matches
accepted_matches = matches.intersection(accepted)
accepted_nonmatches = nonmatches.intersection(accepted)

precision = float(len(accepted_matches))/len(accepted)
recall = float(len(accepted_matches))/len(matches)
fallout = float(len(accepted_nonmatches))/len(nonmatches)

print "\n%s\t%.2f%%\t%.2f%%\t%.2f%%" % ("Overall", 100*precision, 100*recall,100*fallout)

# Calc F-measure
f_measure = 2*precision*recall/(precision+recall)

print "\nF-measure: %.2f%%" % f_measure

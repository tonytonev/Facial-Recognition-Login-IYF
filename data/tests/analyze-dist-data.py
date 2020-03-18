datafile = "/Users/Tony/Dropbox/Code/InYourFace (dropbox)/Trunk/data/test-results/distresults.txt"

# Read data
data = open(datafile).read().split('\n')[2:-1]

# Parse into easy-to-use type
data = [a.split('\t') for a in data]
data = [(a[1], a[6].split('/')[-2], a[6].split('/')[-1], float(a[3]), a[5] == 'True ') for a in data]
data = list(set(data))

users = {}

# Sort data by user in users dict
for d in data:
    if users.has_key(d[0]):
        users[d[0]].append(d)
    else:
        users[d[0]] = [d]

print "Name\tMatch\tNon-Match\tDiff\tMatch Acc\tNonMatch Acc"

for u, v in users.items():
    match_list = [a[3] for a in v if a[-1]]
    nonmatch_list = [a[3] for a in v if not a[-1]]

    match_mean = sum(match_list)/len(match_list)
    nonmatch_mean = sum(nonmatch_list)/len(nonmatch_list)

    threshold = 0.41960943421053

    match_num = len([a for a in v if a[-1]])
    nonmatch_num = len([a for a in v if not a[-1]])

    correct_match_num = len([a for a in v if a[-1] and a[-2] <= threshold])
    correct_nonmatch_num = len([a for a in v if not a[-1] and a[-2] > threshold])

    
    print "%s\t%f\t%f\t%f\t%.0f%%\t%.0f%%" % (u, match_mean, nonmatch_mean, nonmatch_mean - match_mean, 100*float(correct_match_num)/match_num, 100*float(correct_nonmatch_num)/nonmatch_num)

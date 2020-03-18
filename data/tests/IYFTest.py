#!/usr/bin/python

import os, re, subprocess, sys, shlex, random, shutil
from time import time

rootdir = '/Users/Tony/Code/InYourFace/Branches/Tony/data/images/ATT'    #directory of trainingset
dbmodel = '/Users/Tony/Code/InYourFace/Branches/Tony/data/db/IYF-db.sqlite' #path to dbmodel
stdoutfile = '/Users/Tony/Code/InYourFace/Branches/Tony/data/test-results/stdresults.txt'  #logfile
loginresultsfile = '/Users/Tony/Code/InYourFace/Branches/Tony/data/test-results/loginresults.txt' #login results file
distresultsfile = '/Users/Tony/Code/InYourFace/Branches/Tony/data/test-results/distresults.txt' 
whichIYF = '/Users/Tony/Code/InYourFace/Branches/Tony/client/iyf'
iyfcmd = './iyf '
trainmax = 0    #max limit on the range of trainees
maximgpp = 12    #limits the maximum number of images that each person can be trained on
logincnt = 0     #number of login tests to run per trained user
distcnt = 3     #number of distance tests to run per trained user
OFS = '\t'
RFS = '\n'

def clearfilelists():
    if os.path.isfile(stdoutfile):
        os.remove(stdoutfile)
    if os.path.isfile(loginresultsfile):
        os.remove(loginresultsfile)
    if os.path.isfile(distresultsfile):
        os.remove(distresultsfile)
    fd = open(stdoutfile, "w")
    fd.writelines("test run: " + str(time()) + RFS )
    fd.close()
    fd = open(loginresultsfile, "w")
    fd.writelines("test run: " + str(time()) + RFS )
    fd.writelines('number_trainees' + OFS + 'faces_per_trainee' + OFS + 'uname' + OFS + \
                  'uid' + OFS + 'closest_distance' + OFS + \
                  'elapsed_time' + OFS + 'should_login' + OFS + 'cmd' + RFS)
    fd.close()
    fd = open(distresultsfile, "w")
    fd.writelines("test run: " + str(time()) + RFS )
    fd.writelines('number_trainees' + OFS + 'faces_per_trainee' + OFS + 'uname' + OFS + \
                  'uid' + OFS + 'closest_distance' + OFS + \
                  'elapsed_time' + OFS + 'should_login' + OFS + 'cmd' + RFS)
    fd.close()

def bakdbmodel():
    if not os.path.isfile(dbmodel + '.bak'):
        shutil.copyfile(dbmodel, dbmodel + '.bak')

def newdbmodel():
    bakdbmodel()
    os.remove(dbmodel)
    shutil.copyfile(dbmodel + '.bak', dbmodel)

def genlists():
    usrlist = os.listdir(rootdir)
    usrimgs = []
    for user in usrlist:
        imgs = os.listdir(os.path.join(rootdir, user))
        templist = []
        for img in imgs:
            abspath = os.path.join(os.path.join(rootdir, user), img)
            templist.append(abspath)
        usrimgs.append([user, templist])   
    return usrimgs

def gentnglists(imglist, maxipp = maximgpp):
    trainedcnt = 0
    tmax = trainmax
    unused = []
    used = []
    cmdlist = []
        
    if tmax == 0:
        tmax = len(imglist) - 1
        
    for item in imglist:
        uname, ilist = item
        cmd = iyfcmd + '-t ' + uname + ' ' + uname
        imax = maxipp
        if imax == 0:
            imax = len(ilist) - 1 #must have at least one image per person to test against
        elif imax < 2:
            imax = 2
        elif len(ilist) - 1 < maxipp:
            imax = 0
        if trainedcnt < tmax:
            usedimg = []
            unusimg = []
            icnt = 0
            for i in ilist:
                if icnt < imax:
                    cmd = cmd + ' ' + i
                    usedimg.append(i)
                    icnt = icnt + 1
                else:
                    unusimg.append(i)
            used.append([uname, usedimg, icnt])
            unused.append([uname, unusimg])
            cmdlist.append(cmd)
        else: #dont use any more
            unused.append(item)
        trainedcnt = trainedcnt + 1
        
    #for line in used:
    #    print line
    
    return cmdlist, used, unused, tmax
        
def train(cmdlist): #makes system calls given the list of commands (should be 'iyf -t ...' 
    uname = ''
    uid = 0
    usrlst = []
    fd = open(stdoutfile, "a")
    for item in cmdlist:
        args = shlex.split(item)
        fd.writelines("--------------------------------------------------\n")
        fd.writelines(item + '\n')
        proc = subprocess.Popen(args, stdout=subprocess.PIPE)
        uname = shlex.split(item)[2]
        for line in proc.communicate()[0].split('\n'):
            fd.writelines("--> " + line + '\n')
            if re.match("UserId = ", line):
                uid = line[9:]
                usrlst.append([uname, uid])
        try:
            proc.terminate()
        except:
            pass
    fd.close()
    return usrlst
    
        
def testdist(userID, imgFile, shouldmatch = True):
    #fd = open(distresultsfile, "a")
    if imgFile == None:
        print 'imgfile is none'
    if userID == None:
        print 'userId is none'
    cmd = iyfcmd + '-d ' + userID + ' ' + imgFile
    #fd.writelines("--------------------------------------------------\n")
    #fd.writelines(cmd + OFS)
    start = float(format(time(), '.50')) - int(time())
    proc = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE)
    end = float(format(time(), '.50')) - int(time())
    total = end - start
    #print '%s\t%s\t%f'%(end, start, total)
    if total < 0:
        total = 'invalid'
    #print proc.communicate()
    cdist = 'invalid'
    for line in proc.communicate()[0].split('\n'):
        #fd.writelines("--> " + line + '\n')
        if re.match('Distance: ', line):
            cdist = line[10:]
    try:
        proc.terminate()
    except:
        pass
    #fd.writelines(userID + OFS + cdist + OFS + str(total) + OFS + str(shouldmatch) + RFS)
    return cdist, total, cmd
    

def testlogin(userID, imgFile, expected = True):
    fd = open(loginresultsfile, "a")
    cmd = iyfcmd + '-l ' + userID + ' ' + userID + ' ' + imgFile
    fd.writelines("--------------------------------------------------\n")
    fd.writelines(cmd + '\n')
    proc = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE)
    for line in proc.communicate()[0].split('\n'):
        fd.writelines("--> " + line + '\n')
    try:
        proc.terminate()
    except:
        pass
    
def findimg(uname, imglist, match = True, numtrained = trainmax):
    ind = -1
    cnt = 0
    if match:
        for item in imglist[0:numtrained]:
            if item[0] == uname:
                ind = cnt
            else:
                cnt = cnt + 1
        if ind == -1:
            print "------------------------------------------"
            print "findimg failed to find the requested image for user:%s"%uname
            print uname, imglist, match, numtrained
            print "------------------------------------------"
    else:
        ind = random.randint(trainmax, len(imglist) - 1)
    cnum = len(imglist[ind][1])
    return imglist[ind][1][random.randint(0, cnum - 1)]

def main():
    clearfilelists()
    mainlist = genlists()
    os.chdir(os.path.split(whichIYF)[0])
    
    
    ############################################################
    ## use shlex split to break up the cmd list and train on different
    ## number of images per person from 2 to max-1
    ############################################################
    testrange = []
    for x in range(2, maximgpp):
        testrange.append(x)
    testrange.append(0)
    for maxipp in testrange:
        cmdlist, used, unused, trainedtotal = gentnglists(mainlist, maxipp)
                
        if len(cmdlist) == 0:
            print 'breaking due to no more images'
            maxipp = maximgpp + 1
        #cmdlist[index][2] contains uname
        #used[index][0] contains uname
        #unused[index][0] contains uname
        #for line in cmdlist:
        #    print line
        fd = open(distresultsfile, "a")
        for facestrained in range(1, len(cmdlist) + 2):  
            newdbmodel()  
            print maxipp, facestrained
            tmp = train(cmdlist[:facestrained])    
            usrlst = {} 
            for pair in tmp:
                usrlst[pair[0]] = pair[1]
                #trainedtotal = trainedtotal + pair[3]
            #for item in usrlst:
            #    print item, '\t', usrlst.get(item)
    
            #test distances
            for item in cmdlist[:facestrained]:
                uname = shlex.split(item)[2]
                uid = usrlst.get(uname)
                for x in range(0, distcnt):#matching tests
                    dist, elapsed, cmd = testdist(uid, findimg(uname, unused, True, trainedtotal), True)
                    #write output to file
                    #number_trainees uname uid closest_distance elapsed_time should_login cmd
                    fd.writelines(str(facestrained) + OFS + str(maxipp) + OFS + \
                                  uname + OFS + uid + OFS + dist + OFS + \
                                  str(elapsed) + OFS + 'True ' + OFS + cmd + RFS)
                for x in range(0, distcnt):#unmatching tests
                    dist, elapsed, cmd = testdist(uid, findimg(uname, unused, False, trainedtotal), False)
                    fd.writelines(str(facestrained) + OFS + str(maxipp) + OFS + \
                                  uname + OFS + uid + OFS + dist + OFS + \
                                  str(elapsed) + OFS + 'False' + OFS + cmd + RFS)
        fd.close()
    
            #test logins
            #for item in cmdlist:
            #    uname = shlex.split(item)[2]
                #for x in range(0, logincnt):#matching tests
                #    testlogin(uname, findimg(uname, unused, True), 0)
                #for x in range(0, logincnt):#unmatching tests
                #    testlogin(uname, findimg(uname, unused, False), 0)
        
    print 'done'

if __name__ == "__main__":
    main()

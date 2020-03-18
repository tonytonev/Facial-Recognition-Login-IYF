#!/usr/bin/python

import os, re, subprocess, sys, shlex, random, shutil
from time import time

rootdir = os.path.join(os.path.expanduser('~'), 'C:\Documents and Settings\Dave\Desktop\IYF\Trunk\data\TrainSet')    #directory of trainingset
dbmodel = os.path.join(os.path.expanduser('~'), 'C:\Documents and Settings\Dave\Desktop\IYF\Trunk\data\db\IYF-db.sqlite') #path to dbmodel
stdoutfile = os.path.expanduser('C:\Documents and Settings\Dave\Desktop\IYF\Trunk\data\test-results\stdresults.txt')  #logfile
loginresultsfile = os.path.expanduser('C:\Documents and Settings\Dave\Desktop\IYF\Trunk\data\test-results\loginresults.txt') #login results file
distresultsfile = os.path.expanduser('C:\Documents and Settings\Dave\Desktop\IYF\Trunk\data\test-results\distresults.txt') #distance results file
whichIYF = 'C:\Documents and Settings\Dave\Desktop\IYF\Trunk\client\iyf'
iyfcmd = 'C:\Documents and Settings\Dave\Desktop\IYF\Trunk\client\debug\iyf.exe'
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
    fd.writelines('faces_per_trainee' + OFS + 'uname' + OFS + \
                  'closest_distance' + OFS + \
                  'should_login' + OFS + 'did_login' + OFS + \
                  'confidence' + OFS + 'elapsed_time' + OFS + 'cmd' + RFS)
    fd.close()
    fd = open(distresultsfile, "w")
    fd.writelines("test run: " + str(time()) + RFS )
    fd.writelines('number_trainees' + OFS + 'faces_per_trainee' + OFS + 'uname' + OFS + \
                  'closest_distance' + OFS + \
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
    #gets all the images from within the rootdir
    usrlist = os.listdir(rootdir)
    usrimgs = {}
    for user in usrlist:
        imgs = os.listdir(os.path.join(rootdir, user))
        templist = []
        for img in imgs:
            abspath = os.path.join(os.path.join(rootdir, user), img)
            templist.append(abspath)
        usrimgs[user] = templist
    return usrimgs

def gentnglists(imglist, ipp):
    usagepair = {}
    for user in imglist:
        used = []
        unused = []
        used = imglist[user][:ipp]
        unused = imglist[user][ipp: len(imglist[user])]
        usagepair[user] = [used, unused]

    return usagepair

def train(uname, imglist):
    #print 'train: ', imglist
    usrlst = []
    fd = open(stdoutfile, "a")
    cmd = iyfcmd + ' -t ' + uname + ' ' + uname
    for img in imglist:
        cmd = cmd + ' ' + img
        args = shlex.split(cmd)
        fd.writelines("--------------------------------------------------\n")
        fd.writelines(cmd + '\n')
        proc = subprocess.Popen(args, stdout=subprocess.PIPE)
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

#def testdist(userID, imgFile, shouldmatch = True):
#    #fd = open(distresultsfile, "a")
#    if imgFile == None:
#        print 'imgfile is none'
#    if userID == None:
#        print 'userId is none'
#    cmd = iyfcmd + '-d ' + userID + ' ' + imgFile
#    #fd.writelines("--------------------------------------------------\n")
#    #fd.writelines(cmd + OFS)
#    start = float(format(time(), '.50')) - int(time())
#    proc = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE)
#    end = float(format(time(), '.50')) - int(time())
#    total = end - start
#    #print '%s\t%s\t%f'%(end, start, total)
#    if total < 0:
#        total = 'invalid'
#    #print proc.communicate()
#    cdist = 'invalid'
#    for line in proc.communicate()[0].split('\n'):
#        #fd.writelines("--> " + line + '\n')
#        if re.match('Distance: ', line):
#            cdist = line[10:]
#    try:
#        proc.terminate()
#    except:
#        pass
#    #fd.writelines(userID + OFS + cdist + OFS + str(total) + OFS + str(shouldmatch) + RFS)
#    return cdist, total, cmd


def testlogin(uname, imgFile, expected = True):
    #fd = open(loginresultsfile, "a")
    cmd = iyfcmd + '-l ' + uname + ' ' + uname + ' ' + imgFile
    #fd.writelines("--------------------------------------------------\n")
    #fd.writelines(cmd + '\n')
    start = float(format(time(), '.50')) - int(time())
    proc = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE)
    comms = proc.communicate()[0].split('\n')
    end = float(format(time(), '.50')) - int(time())
    total = end - start
    #for line in comms.split('\n'):
    #    fd.writelines("--> " + line + '\n')
    try:
        proc.terminate()
    except:
        pass
    return comms, total, cmd

def findimg(uname, imglist, match = True, max = 8):
    if match:
        candidates = imglist[uname][1]
        return candidates
    else:
        candidates = []
        keys = imglist.keys()
        ind = random.randint(0, len(keys) - 1)
        for cnt in range(0, max):
            while keys[ind] == uname:
                ind = random.randint(0, len(keys) - 1)
            select = imglist[keys[ind]][random.randint(0, len(imglist[keys[ind]]) - 1)]
            while select in candidates:
                select = imglist[keys[ind]][random.randint(0, len(imglist[keys[ind]]) - 1)]
            candidates.append(select)

    return candidates

def findnumunused(imglist, uname):
    for item in imglist:
        if item[0] == uname:
            return len(item[1])
    return 0

def interploginfeedback(lines):
    success = 'X'
    confid = 0
    closest = 10
    for line in lines:
        if re.match('Return', line):
            if re.search('successful', line):
                success = True
            elif re.search('fail', line):
                success = False
        if re.match('Confidence level: ', line):
            confid = float(line[18:])
        if re.match('The least distance returned is ', line):
            closest = float(line[31:])
    return success, confid, closest

def main():
    clearfilelists()
    mainlist = genlists()
    os.chdir(os.path.split(whichIYF)[0])

    fdl = open(loginresultsfile, "a")
    for cnt in 9, 8, 7, 6, 5, 4, 3, 2:
       # print cnt
        usagepair = gentnglists(mainlist, cnt)
        i = 1
        for uname in usagepair:
            train(uname, usagepair[uname][0])
           # print cnt, i
            i = i + 1
            #login testing only#
            #successful testing
            for img in usagepair[uname][1]: #test against all 'good'images
                comms, elapsed, cmd = testlogin(uname, img)
                succ, conf, clos = interploginfeedback(comms)
                #faces_per_trainee    uname    uid    closest_distance    elapsed_time    should_login    cmd
                fdl.writelines(str(cnt) + OFS + uname + OFS + str(clos) + OFS + \
                              OFS + 'True' + OFS + str(succ) + OFS + str(conf) + OFS + \
                              str(elapsed) + OFS + cmd + RFS)
            #fail testing
            for img in findimg(uname, mainlist, False, 8): #tests against 8 'bad' images
                comms, elapsed, cmd = testlogin(uname, img)
                succ, conf, clos = interploginfeedback(comms)
                #faces_per_trainee    uname    uid    closest_distance    elapsed_time    should_login    cmd
                fdl.writelines(str(cnt) + OFS + uname + OFS + str(clos) + OFS + \
                              OFS + 'False' + OFS + str(succ) + OFS + str(conf) + OFS + \
                              str(elapsed) + OFS + cmd + RFS)
            ####################
#    for item in usagepair:
#        used = item[0]
#        unused = item[1]
#        cmd = iyfcmd + ' -t genu genp'
#        for item in used:
#            newdbmodel()
#            cmd = cmd + ' ' + item
#            train(cmd)

            #to do
            #run test distances against all unused
            #figure out how to determine if an image should be accepted or not
            #based on whether or an image has been used from the same directory


    fdl.close()
   # print 'done'

if __name__ == "__main__":
    main()
#!C:\Perl\bin\perl.exe

sub choose5
{
	my ($min, $max) = @_;
	my @chosen;
	for(my $i1 = $min; $i1 <= $max; $i1++)
	{
		for(my $i2 = $i1+1; $i2 <= $max; $i2++)
		{
			for(my $i3 = $i2+1; $i3 <= $max; $i3++)
			{
				for(my $i4 = $i3+1; $i4 <= $max; $i4++)
				{
					for(my $i5 = $i4+1; $i5 <= $max; $i5++)
					{
						push (@chosen, [($i1,$i2,$i3,$i4,$i5)]);
					}
				}
			}
		}
	}
	return @chosen;
}

sub diff
{
	(my $min, my $max, my @from) = @_;
	my @result;
	my $j = 0;
	for(my $i = $min; $i <= $max; $i++)
	{
		if($from[$j] == $i)
		{
			$j++;
		}
		else
		{
			push(@result, $i);
		}
	}
	return @result;
}

my $iyfExec = '"../../client/iyf.exe"';
my $ulogin = 'test';
my $upass = 'test';

my $testFolderStart = 7;
my $testImageStart = 1;
my $testImageEnd = 10;
my $logDir = '../test-results';
my $resultLog = $logDir.'/dtai-login-results.log';
my $commandLog = $logDir.'/dtai-commands.log';
my $imageDir = '../images';
my $aTTImageDir = $imageDir.'/ATT';

open(RESULTSFILE, '>'.$resultLog);
open(COMMLOG, '>'.$commandLog);
opendir(TESTDIR, $aTTImageDir) || die "Error in opening $aTTImageDir\n";

readdir(TESTDIR);
readdir(TESTDIR);
for(my $i = 0; $i < $testFolderStart; $i++)
{
	readdir(TESTDIR);
}

my @loginSets;
my @trainingSets;
my @chosen = choose5($testImageStart, $testImageEnd);

while(my $subDir = readdir(TESTDIR))
{
	foreach my $chose (@chosen)
	{
		my @trainingSet = ();
		my @loginSet = ();
		
		foreach my $chose2 (@{$chose})
		{
			push(@trainingSet, $aTTImageDir.'/'.$subDir.'/'.$chose2.'.pgm');
		}
		@loginNumbers = diff($testImageStart, $testImageEnd, @{$chose});
		foreach my $login (@loginNumbers)
		{
			push(@loginSet, $aTTImageDir.'/'.$subDir.'/'.$login.'.pgm');
		}
		$trainExec = $iyfExec.' -t '.$ulogin.' '.$upass.' '.join(' ', @trainingSet);
	    print "$trainExec\n";
		my $trainStartTime = time();
		`$trainExec`;
		my $trainEndTime = time();
		print COMMLOG ($trainEndTime - $trainStartTime)." $trainExec\n";
		foreach my $login (@loginSet)
		{
			$loginExec = $iyfExec.' -l '.$ulogin.' '.$upass.' '.$login;
			print "\t$loginExec";
			my $loginStartTime = time();
			my $result = `$loginExec`;
			my $loginEndTime = time();
			print " $result\n";
			print COMMLOG ($loginEndTime - $loginStartTime)." $result\t$loginExec\n";
		}
		exit();
	}
}

#$trainingExec = join(' ',@trainingSet);
#foreach $trainingSet (@trainingSets)
#{
#	print $trainingSet;
#}

close(RESULTFILE);
close(COMMLOG);
closedir(TESTDIR);
% //col 1 is height
% //col 2 is weight
% //col 3 is wingspan
% //col 4 is reach
% //col 5 is no step vert
% //col 6 is no step vert reach
% //col 7 is running vert
% //col 8 is running vert reach
% //col 9 is draft or not


%%%%% INTRO STUFF %%%%

%%%% 1st distributions

%%% height
%%% wingspan
%%% vertical standing
%%% vertical running

data = csvread('realFinalData.csv');

height=data(:,1);
wingspan=data(:,3);
vertStand=data(:,5);
vertRunning=data(:,7)

hist(height)
title('Distribution of Height');
figure;
hist(wingspan)
title('Distribution of Wingspan');
figure;
hist(vertStand)
title('Distribution of Standing Vertical Leap');
figure;
hist(vertRunning)
title('Distribution of Running Vertical Leap');
figure;

%%% end distributions


%%% see first of all what percantages of all draftees got into NBA %%%


drafted=data(:,9);
in=0;
for i=1:1384
	if(drafted(i)>0)
		in=in+1;
		boolDraft(i)=1;
	else
		boolDraft(i)=0;
	end
end
out=1384-in;
 %% percent chance of being in NBA if you get invited to combine

percOut=out/1384*100
percIn=in/1384*100

str=sprintf('%.1f',percIn);
s=strcat('Drafted %',str);
str1=sprintf('%.1f',percOut);
s1=strcat('Undrafted %',str1);

mat=[in;out];
labels = {s,s1};
explode=[1 0];
p=pie3(mat,explode,labels);

hp = findobj(p, 'Type', 'patch');

set(hp(2), 'FaceColor', 'g');




%%% now see if theres any relation between height and getting drafted
%%% since most people think being short you have no chance 

corrcoef([height boolDraft'])
%% almost 0 correlation coefficient 


%%% averages of key stats for groups like drafted or not drafted

mean(height)
mean(vertRunning)

%% now the same measurments of only the ones who are drafted
cur=1;
cur2=1;
for i=1:1384
	if(boolDraft(i)==1)
		onlyDrafted(cur,1:9)=data(i,:);
		cur=1+cur;
	else
		notDrafted(cur2,1:9)=data(i,:);
		cur2=cur2+1;
	end
end

draftedHeight=onlyDrafted(:,1);
draftedRunVert=onlyDrafted(:,7); 
draftedWingspan=onlyDrafted(:,3); 
draftedReach=onlyDrafted(:,4); 

notDraftedHeight=notDrafted(:,1);
notDraftedRunVert=notDrafted(:,7); 
notDraftedWingspan=notDrafted(:,3); 
notDraftedReach=notDrafted(:,4); 



mean(draftedHeight)
mean(draftedRunVert) %% higher by 1.58 inches
mean(draftedWingspan)
mean(draftedReach)

mean(notDraftedHeight)
mean(notDraftedRunVert)
mean(notDraftedWingspan)
mean(notDraftedReach)


%% the max running vert is higher for that of those not drafted

%%%%%%%%%%%%% NOW MEASURE other athletic abilities in comparison
dataNBF=csvread('dataWithoutBodyFat.csv');
dataBF=csvread('dataWithBodyFat.csv');

%size 966

draft=dataNBF(:,12);
draftFat=dataBF(:,13);

cur=1;
cur2=1;
for i=1:966
	if(draft(i)>0)
		noBFdrafted(cur,1:12)=dataNBF(i,:);
		cur=1+cur;
	else
		noBFundrafted(cur2,1:12)=dataNBF(i,:);
		cur2=cur2+1;
	end
end

cur=1;
cur2=1;
for i=1:638
	if(draftFat(i)>0)
		BFdrafted(cur,1:13)=dataBF(i,:);
		cur=1+cur;
	else
		BFundrafted(cur2,1:13)=dataBF(i,:);
		cur2=cur2+1;
	end
end

vertDraft=noBFdrafted(:,7);
vertUnDraft=noBFundrafted(:,7);

mean(vertDraft)
mean(vertUnDraft)

benchDraft=noBFdrafted(:,9);
agilityDraft=noBFdrafted(:,10);
sprintDraft=noBFdrafted(:,11);P
draftDraft=noBFdrafted(:,12);

benchUnDraft=noBFundrafted(:,9);
agilityUnDraft=noBFundrafted(:,10);
sprintUnDraft=noBFundrafted(:,11);
draftUnDraft=noBFundrafted(:,12);

bodyFatDrafted=BFdrafted(:,5); %size 638
bodyFatUndrafted=BFundrafted(:,5);

mean(bodyFatDrafted)
mean(bodyFatUndrafted)


mean(benchDraft)
mean(benchUnDraft)

mean(agilityDraft)
mean(agilityUnDraft)

mean(sprintDraft)
mean(sprintUnDraft)


leapFatUndrafted=BFundrafted(:,8);
leapFatdrafted=BFdrafted(:,8);
mean(leapFatdrafted)
mean(leapFatUndrafted)

%%%%% END INTRO STUFF %%%%%% 


%%%% BEGIN ATHLETIC FORMULA

data=csvread('dataWithoutBodyFat.csv');


height=data(:,1);
weight=data(:,2);
vert=data(:,7);
bench=data(:,9);
agility=data(:,10);
sprint=data(:,11);
draft=data(:,12);




%adjust vert first 
for i=1:966
	adjVert(i)=vert(i)/weight(i);
end
%adjust sprint speed 
for i=1:966
	stepSize=1.22*height(i);
	stepsTaken=75/stepSize;
	footSpeed=stepsTaken/sprint(i);
	adjSpeed(i)=footSpeed;
end

%adjusted bench press
for i=1:966
	if(bench(i)==0)
		adjBench(i)=0;
		continue;
	end
	if(bench(i)==1)
		adjBench(i)=0;
		continue;
	end
	if(weight(i)<=185)
		adjBench(i)=bench(i);
		continue;
	end
	if(weight(i)>185)
		adjBench(i)=(185/weight(i))*bench(i);
		continue;
	end  
end

%adjusted agility 
for i=1:966
	stepSize=1.22*height(i);
	mid=(140/agility(i))*(1/stepSize);
	adjAgility(i)=(8/weight(i))*mid;
	adjAgility(i)=mid;
end


newData=[height weight data(:,3) data(:,4) data(:,5) data(:,6) adjVert' data(:,8) adjBench' adjAgility' adjSpeed' draft];

%now adjust each score to be a grade out 10 so we can add them all up

sortV=sort(adjVert);
sortSpeed=sort(adjSpeed);
sortAgility=sort(adjAgility);
sortBench=sort(adjBench);

minVert=sortV(50);
maxVert=sortV(900);

minSprint=sortSpeed(50);
maxSprint=sortSpeed(900);

minAgility=sortAgility(50);
maxAgility=sortAgility(900);

minBench=sortBench(50);
maxBench=sortBench(900);


for i=1:966
	benchPt=adjBench(i);
	agilityPt=adjAgility(i);
	sprintPt=adjSpeed(i);
	vertPt=adjVert(i);

	scaledBench(i)= (10*(benchPt-minBench))/(maxBench-minBench);
	if(scaledBench(i)<0)
		scaledBench(i)=0;
	end
	if(scaledBench(i)>10)
		scaledBench(i)=10;
	end

	scaledAgility(i)=(10*(agilityPt-minAgility))/(maxAgility-minAgility);
	if(scaledAgility(i)<0)
		scaledAgility(i)=0;
	end
	if(scaledAgility(i)>10)
		scaledAgility(i)=10;
	end 


	scaledSprint(i)=(10*(sprintPt-minSprint))/(maxSprint-minSprint);
	if(scaledSprint(i)<0)
		scaledSprint(i)=0;
	end
	if(scaledSprint(i)>10)
		scaledSprint(i)=10;
	end 

	scaledVert(i)=(10*(vertPt-minVert))/(maxVert-minVert);
	if(scaledVert(i)<0)
		scaledVert(i)=0;
	end
	if(scaledVert(i)>10)
		scaledVert(i)=10;
	end 



	mid=scaledBench(i)+scaledVert(i)+(scaledAgility(i))+(scaledSprint(i));
	athleticScore(i)=mid/4;	
	if(athleticScore(i)<2)
		athleticScore(i)=(height(i))/2;
	end
end


inAbove5=0;
inBelow5=0;

outAbove5=0;
outBelow5=0;
for i=1:966
	if(athleticScore(i)>5)
		if(draft(i)>0)
			inAbove5=inAbove5+1;
			scatter(athleticScore(i),1,'ro');
		else
			outAbove5=outAbove5+1;
			scatter(athleticScore(i),1,'bx');
		end 
	
	hold on;
else 
		if(draft(i)>0)
			inBelow5=inBelow5+1;
			scatter(athleticScore(i),1,'ro');
		else
			outBelow5=outBelow5+1;
			scatter(athleticScore(i),1,'bx');
		end 
	
	hold on;
end 
end
figure; 

in=inAbove5+inBelow5;
out=outAbove5+outBelow5;
for i=1:966
	if(draft(i)>0)
		adjDraft(i)=1;
	else
		adjDraft(i)=0;
	end
end

cur=1;
cur1=1;
for i=1:966
	if(draft(i)>0)
		dataIn(cur)=athleticScore(i);
		cur=cur+1;
	else
		dataOut(cur1)=athleticScore(i);
		cur1=cur1+1;
	end
end
hIn=hist(dataIn)
title('Drafted NBA');
figure;
hOut=hist(dataOut)
title('Not Drafted');


hist(dataIn)
title('Drafted NBA');
figure;
hist(dataOut)
title('Not Drafted');
%%%%%%%%%%%% taking into accoutn all players no difference



%%%%% now looking into players by position
data=csvread('dataWithoutBodyFat.csv');

height=data(:,1);

cur=1;
for i=1:966
	if(height(i)<=6.41)
		guards(cur,1:12)=data(i,:);
		cur=cur+1;
	end
end

cG=corr(guards);
[U,S,V]=svd(cG);

firstG=U(:,1);
secondG=U(:,2);
plotFG1=guards*firstG;
plotFG2=guards*secondG;
guardDraft=guards(:,12);
%sze is 348
for i=1:384
	if(guardDraft(i)>0)
	scatter(plotFG1(i),plotFG2(i),'ro') 
	hold on;
	else 
	scatter(plotFG1(i),plotFG2(i),'bx')
	hold on;
	end
end 
hold on;

%% least squares fit guards
A=[plotFG1.^3 plotFG1.^2 plotFG1.^1 plotFG1.^0];
b=plotFG2;
x=A\b;
p=[x'];
d=linspace(min(plotFG1),max(plotFG2),384);
y=polyval(p,d);
hold on;
plot(d,y,'k');

inAbove=0;
inBelow=0;

outAbove=0;
outBelow=0;

for i=1:384
	y=polyval(p,plotFG1(i));
	if(plotFG2(i)>y)%%ABOVE the fit line
		if(guardDraft(i)>0)
			inAbove=inAbove+1;
		else
			outAbove=outAbove+1;
		end 
	else
		if(guardDraft(i)>0)
			inBelow=inBelow+1;
		else
			outBelow=outBelow+1;
		end 
	end 
end

inAboveG=inAbove;
inBelowG=inBelow;
outAboveG=outAbove;
outBelowG=outBelow;


figure;


%% forwards below 6'9.5" 489

cur=1;
for i=1:966
	if((height(i)>6.41) && (height(i)<6.79))
		forwards(cur,1:12)=data(i,:);
		cur=cur+1;
	end
end 
cF=corr(forwards);
[UF,S1,V1]=svd(cF);

firstF=UF(:,1);
secondF=UF(:,2);
plotFF1=forwards*firstF;
plotFF2=forwards*secondF;
forDraft=forwards(:,12);
%sze is489
for i=1:453
	if(forDraft(i)>0)
	scatter(plotFF1(i),plotFF2(i),'ro') 
	hold on;
	else 
	scatter(plotFF1(i),plotFF2(i),'bx')
	hold on;
	end
end 

hold on;

%%%%repeat forForwards
A=[plotFF1.^3 plotFF1.^2 plotFF1.^1 plotFF1.^0];
b=plotFF2;
x=A\b;
p=[x'];
d=linspace(min(plotFF1),max(plotFF2),453);
y=polyval(p,d);
hold on;
plot(d,y,'k');

inAbove=0;
inBelow=0;

outAbove=0;
outBelow=0;

for i=1:384
	y=polyval(p,plotFF1(i));
	if(plotFF2(i)>y)%%ABOVE the fit line
		if(forDraft(i)>0)
			inAbove=inAbove+1;
		else
			outAbove=outAbove+1;
		end 
	else
		if(forDraft(i)>0)
			inBelow=inBelow+1;
		else
			outBelow=outBelow+1;
		end 
	end 
end

inAboveF=inAbove;
inBelowF=inBelow;
outAboveF=outAbove;
outBelowF=outBelow;

figure;





%% centers above 6 9.5 164
cur=1;
for i=1:966
	if((height(i)>=6.75))
		centers(cur,1:12)=data(i,:);
		cur=cur+1;
	end
end

centerDraft=centers(:,12);
cC=corr(centers);
[UC,S,V]=svd(cC);
firstC=UC(:,1);
secondC=UC(:,2);
plotC1=centers*firstC;
plotC2=centers*secondC;

for i=1:164
	if(centerDraft(i)>0)
	scatter(plotC1(i),plotC2(i),'ro') 
	hold on;
	else
		scatter(plotC1(i),plotC2(i),'bx') 
	hold on;
	end
end

hold on;
% plotFG1.^4  

%%%%repeat for centers
A=[plotC1.^1 plotC1.^0];
b=plotC2;
x=A\b;
p=[x'];
d=linspace(min(plotC1),max(plotC2),164);
y=polyval(p,d);
hold on;
plot(d,y,'k');
hold on;

inAbove=0;
inBelow=0;

outAbove=0;
outBelow=0;

for i=1:164
	y=polyval(p,plotC1(i));
	if(plotC2(i)>y)%%ABOVE the fit line
		if(centerDraft(i)>0)
			inAbove=inAbove+1;
			scatter(plotC1(i),plotC2(i),'ro')
			hold on;
		else
			scatter(plotC1(i),plotC2(i),'bo')
			outAbove=outAbove+1;
			hold on;
		end 
	else
		if(centerDraft(i)>0)
			scatter(plotC1(i),plotC2(i),'rx')
			inBelow=inBelow+1;
			hold on;
		else
			scatter(plotC1(i),plotC2(i),'bx')
			outBelow=outBelow+1;
			hold on;
		end 
	end 
end


inAboveC=inAbove;
inBelowC=inBelow;
outAboveC=outAbove;
outBelowC=outBelow;

%%%% wingspan plots



dataNoBf=csvread('dataWithoutBodyFat.csv');


data=dataNoBf;

draft=data(:,12);
height=data(:,1);
wingspan=data(:,3);

cur=5.5;
for i=1:966
	x(i)=cur;
	cur=cur+0.002;
end

plot(x,x*1.029,'k');
hold on;


in=0;
out=0;
for i=1:966
	if((draft(i)>0))
	scatter(height(i),wingspan(i),'bx');
	in=in+1;
	hold on;
	end
	if((draft(i)<=0))
	scatter(height(i),wingspan(i),'ro');
	out=out+1;
	hold on;
	end
end

title('NBA Wingspans vs Average Wingspans')


data = csvread('realFinalData.csv');
wingspan=data(:,3);
w=mean(wingspan);
h=mean(height);
w-h

%%%%%%%% plots of average draftees

data = csvread('realFinalData.csv');

draft=data(:,9);
cur=1;
for i=1:1384
	if(draft(i)>0)
		draftOnly(cur,1:9)=data(i,:);
		cur=cur+1;
	end 
end 
data=draftOnly;

height=data(:,1);
weight=data(:,2);
vert=data(:,7);


scatter3(height,weight,vert);

title('Historically Drafted Players Data');
xlabel('height');
ylabel('weight');
zlabel('vertical');
figure;
%%%%%% 3d plot of both variables

%%% go for weight against vertical

A=[weight.^1 weight.^0];
b=vert;
x=A\b;
p=[x'];
weightFactors=p;
d=linspace(min(weight),max(weight),164);
y=polyval(p,d);
scatter(weight,vert);
hold on;
plot(d,y,'k');
title('Historically Drafted Players Data');
xlabel('weight');
ylabel('vert');


%%% go for height against vertical
figure;

A=[height.^1 height.^0];
b=vert;
x=A\b;
p=[x'];
heightFactors=p;
d=linspace(min(height),max(height),164);
y=polyval(p,d);
scatter(height,vert);
hold on;
plot(d,y,'k');
title('Historically Drafted Players Data');
xlabel('height');
ylabel('vert');


%%% creating a random lebron james

players=0;
done=0;
while (done == 0)
% make random height 
% between 5.5 and 7.5
%open interval a and b
players=players+1;

a = 5.5;
b = 7.5;
randomHeights = (b-a).*rand(1,1) + a;
	if(randomHeights>=6.75)
		% make random running vertical
% between 20 inches and 50 inches
%open interval a and b
		a = 20.0;
		b = 50.0;
		randomVert = (b-a).*rand(1,1) + a;
			if(randomVert>=44)
				% make random bench press
				% the least a player lifted is 0 and a
				% record is 30
				a = 0.0;
				b = 30.0;
				randomBench = (b-a).*rand(1,1) + a;
				if(randomBench>10)
					%make a random agility
						%2.91
						a = 9.0;
						b = 15.0;
						randomAgility = (b-a).*rand(1,1) + a;
						if(randomAgility<=10.3)
							%make a random sprint speed
								a = 2.8;
								b = 4.4
								randomSprint = (b-a).*rand(1,1) + a;
								if(randomSprint<=3.05)
									done=1;
								end

						end 

				end 
			end
	end







end


years=players/60;





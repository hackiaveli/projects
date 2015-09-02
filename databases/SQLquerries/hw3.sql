/* CS 143 Spring 2014, Homework 3 - Federal Government Shutdown Edition */

/*******************************************************************************
 For each of the queries below, put your SQL in the place indicated by the
 comment.  Be sure to have all the requested columns in your answer, in the
 order they are listed in the question - and be sure to sort things where the
 question requires them to be sorted, and eliminate duplicates where the
 question requires that.  We will grade the assignment by running the queries on
 a test database and eyeballing the SQL queries where necessary.  We won't grade
 on SQL style, but we also won't give partial credit for any individual question
 - so you should be confident that your query works. In particular, your output
 should match our example output in hw3trace.txt
********************************************************************************/

/*******************************************************************************
 Q1 - Return the statecode, county name and 2010 population of all counties who
 had a population of over 2,000,000 in 2010. Return the rows in descending order
 from most populated to least
 ******************************************************************************/

/* Put your SQL for Q1 here */

SELECT statecode , name , population_2010
FROM counties
WHERE population_2010 > 2000000 
ORDER BY population_2010 DESC;

/*******************************************************************************
 Q2 - Return a list of statecodes and the number of counties in that state,
 ordered from the least number of counties to the most 
*******************************************************************************/

/* Put your SQL for Q2 here */

SELECT states.statecode, COUNT(*) 
FROM states, counties
WHERE states.statecode = counties.statecode
GROUP BY states.statecode
ORDER BY COUNT(*) ASC ;

/*******************************************************************************
 Q3 - On average how many counties are there per state (return a single real
 number) 
*******************************************************************************/

/* Put your SQL for Q3 here */

SELECT AVG(t.total) as 'Average Num of Counties Per State'
FROM
(SELECT COUNT(*) as total
FROM states, counties
WHERE states.statecode = counties.statecode
GROUP BY states.name) as t ;


/*******************************************************************************
 Q4 - return a count of how many states have more than the average number of
 counties
*******************************************************************************/

/* Put your SQL for Q4 here */

SELECT COUNT(*)
FROM
(
SELECT states.statecode, COUNT(*) as countyCount
FROM states, counties
WHERE states.statecode = counties.statecode
GROUP BY states.statecode
) as total
WHERE countyCount >
(
SELECT AVG(t.total) as 'Average Num of Counties Per State'
FROM
(SELECT COUNT(*) as total
FROM states, counties
WHERE states.statecode = counties.statecode
GROUP BY states.name) as t) ;



/*******************************************************************************
 Q5 - Data Cleaning - return the statecodes of states whose 2010 population does
 not equal the sum of the 2010 populations of their counties
*******************************************************************************/

/* Put your SQL for Q5 here */


SELECT sumCounties.statecode
FROM
(SELECT states.statecode, SUM(counties.population_2010) as sumPops, states.population_2010
FROM states, counties
WHERE states.statecode = counties.statecode
GROUP BY states.statecode)as sumCounties
WHERE sumCounties.population_2010 <> sumCounties.sumPops;



/*******************************************************************************
 Q6 - How many states have at least one senator whose first name is John,
 Johnny, or Jon? Return a single integer
*******************************************************************************/

/* Put your SQL for Q6 here */

SELECT COUNT(DISTINCT senators.statecode)
FROM states,senators
WHERE states.statecode = senators.statecode AND
(senators.name LIKE "John%" OR senators.name LIKE "Johnny%" OR senators.name LIKE "Jon%");


/*******************************************************************************
Q7 - Find all the senators who were born in a year before the year their state
was admitted to the union.  For each, output the statecode, year the state was
admitted to the union, senator name, and year the senator was born.  Note: in
SQLite you can extract the year as an integer using the following:
"cast(strftime('%Y',admitted_to_union) as integer)"
*******************************************************************************/
SELECT senators.statecode,YEAR(states.admitted_to_union),senators.name,senators.born
FROM states,senators
WHERE senators.statecode = states.statecode AND YEAR(states.admitted_to_union) > senators.born;



/*******************************************************************************
Q8 - Find all the counties of West Virginia (statecode WV) whose population
shrunk between 1950 and 2010, and for each, return the name of the county and
the number of people who left during that time (as a positive number).
*******************************************************************************/

/* Put your SQL for Q8 here */

SELECT counties.name, counties.population_1950 - counties.population_2010 AS PopulationDifference
FROM counties, states
WHERE (counties.population_1950 > counties.population_2010) AND counties.statecode = 'WV'
GROUP BY counties.name;




/*******************************************************************************
Q9 - Return the statecode of the state(s) that is (are) home to the most
committee chairmen
*******************************************************************************/

/* Put your SQL for Q9 here */

SELECT states.statecode
FROM
(SELECT *
FROM
(SELECT MAX(subQ.count) as 'realMax'
FROM committees,senators,
(
SELECT senators.statecode as 'stateCodes', COUNT(committees.chairman) as 'count'
FROM committees,senators
WHERE committees.chairman = senators.name
GROUP BY senators.statecode
)as subQ
WHERE committees.chairman = senators.name) as subQ1,
(
SELECT senators.statecode as 'stateCodes', COUNT(committees.chairman) as 'count'
FROM committees,senators
WHERE committees.chairman = senators.name
GROUP BY senators.statecode
)as subQ2) as final,states
WHERE states.statecode = final.stateCodes AND final.realMax = final.count;




/*******************************************************************************
Q10 - Return the statecode of the state(s) that are not the home of any
committee chairmen
*******************************************************************************/

/* Put your SQL for Q10 here */

SELECT states.statecode
FROM states 
WHERE states.statecode NOT IN
(SELECT states.statecode
FROM committees,senators,states
WHERE committees.chairman = senators.name AND senators.statecode = states.statecode
GROUP BY senators.statecode);

/*******************************************************************************
Q11 Find all subcommittes whose chairman is the same as the chairman of its
parent committee.  For each, return the id of the parent committee, the name of
the parent committee's chairman, the id of the subcommittee, and name of that
subcommittee's chairman
*******************************************************************************/

/*Put your SQL for Q11 here */

SELECT parent.parentId, parent.parentChair ,children.subComId  ,children.subComChairman 
FROM
(SELECT parent_committee as 'ParentCom', id as 'subComId', chairman as 'subComChairman'
FROM committees
WHERE committees.id IN
(SELECT committees.id
FROM committees
WHERE committees.parent_committee LIKE "%")) as children,
(SELECT chairman as 'parentChair', id as 'parentId'
FROM committees
WHERE committees.id NOT IN
(SELECT committees.id
FROM committees
WHERE committees.parent_committee LIKE "%")) as parent
WHERE parent.parentChair = children.subComChairman AND parent.parentID = children.ParentCom;

/*******************************************************************************
Q12 - For each subcommittee where the subcommittee‚Äôs chairman was born in an
earlier year than the chairman of its parent committee, Return the id of the
parent committee, its chairman, the year the chairman was born, the id of the
submcommittee, it‚Äôs chairman and the year the subcommittee chairman was born.
********************************************************************************/

/* Put your SQL for Q12 here */
SELECT parents.comId, parents.chairman,parents.born,children.subComId,children.chairman,children.born
FROM
(SELECT id as 'ComId', chairman,born
FROM committees,senators
WHERE committees.id NOT IN
(SELECT committees.id
FROM committees
WHERE committees.parent_committee LIKE "%") AND senators.name=committees.chairman) as parents,
(SELECT id as 'subComId', chairman,born,parent_committee as 'parentCom'
FROM committees,senators
WHERE committees.id  IN
(SELECT committees.id
FROM committees
WHERE committees.parent_committee LIKE "%") AND senators.name=committees.chairman) as children
WHERE children.born < parents.born and children.parentCom=parents.ComId
ORDER BY parents.ComId;

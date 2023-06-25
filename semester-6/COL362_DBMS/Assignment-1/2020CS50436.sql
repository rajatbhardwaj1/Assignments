--1--
SELECT
    people.playerID, namefirst AS firstname, namelast AS lastname, SUM( CS ) AS total_caught_stealing
FROM
    batting, people
WHERE
    batting.playerID = people.playerID
AND
    CS IS NOT NULL
GROUP BY
    people.playerID, namefirst, namelast
ORDER BY 
    total_caught_stealing DESC , firstname DESC ,lastname , playerID 
LIMIT 10;

--2--
SELECT 
    batting.playerID, namefirst AS firstname, SUM(COALESCE(H2B , 0 )*2 + COALESCE(H3B, 0 )*3 + COALESCE(HR, 0)*4 ) AS runscore
FROM
    batting, people
WHERE 
    batting.playerID = people.playerID
GROUP BY
    batting.playerID , namefirst 
ORDER BY
    runscore DESC , firstname DESC , playerID 
LIMIT 10 ;

--3--
SELECT 
    people.playerID , namefirst||' '||namelast AS playername, sum(pointsWon) AS total_points
FROM
    AwardsSharePlayers, people
WHERE
    AwardsSharePlayers.playerID  = people.playerID
AND
    AwardsSharePlayers.yearID >= 2000
GROUP BY
    people.playerID, playername
ORDER BY    
    total_points DESC, playerID ;

--4--
WITH 
    num_season
AS 
(
    SELECT 
        playerID , COUNT(DISTINCT yearID) AS season_ct
    FROM
        batting
    WHERE
        AB IS NOT NULL
    AND
        H IS NOT NULL
    AND
        AB != 0 
    GROUP BY
        playerID
)

SELECT 
    batting.playerID, namefirst AS firstname , namelast  AS lastname , AVG(H::float/COALESCE(nullif(AB , 0) , 1)) AS career_batting_average
FROM
    num_season, batting , people
WHERE
    num_season.playerID = batting.playerID
AND
    people.playerID = batting.playerID
AND 
    num_season.season_ct >= 10 
AND
    AB != 0 
GROUP BY
    batting.playerID , firstname ,lastname 
ORDER BY 
    career_batting_average DESC, playerID , firstname , lastname 
LIMIT 10; 


--5--
WITH 
    player_Union
AS 
(
    SELECT 
        playerID, yearID
    FROM 
        batting
    UNION
    SELECT 
        playerID, yearID
    FROM 
        pitching
    UNION
    SELECT 
        playerID, yearID
    FROM 
        fielding
    
)
SELECT 
    player_Union.playerID, namefirst AS firstname , namelast AS lastname , DATE(birthYear||'-'||birthMonth||'-'||birthDay )AS date_of_birth , COUNT(yearID) AS num_season
FROM
    people, player_Union 
WHERE 
    people.playerID = player_Union.playerID
GROUP BY
    player_Union.playerID, firstname , lastname , date_of_birth 
ORDER BY    
    num_season DESC, player_Union.playerID, firstname , lastname , date_of_birth ;

--6--
WITH 
    team_div_win
AS
(
    SELECT 
        teamID, yearID 
    FROM
        teams
    WHERE
        DivWin = true 
)
SELECT 
    team_div_win.teamID, name , franchName , MAX(W) AS numwins
FROM
    teams, TeamsFranchises, team_div_win
WHERE
    teams.franchID = TeamsFranchises.franchID
AND
    team_div_win.teamID = teams.teamID
AND
    team_div_win.yearID = teams.yearID
GROUP BY
    team_div_win.teamID, name, franchName
ORDER BY
    numwins DESC, team_div_win.teamID ,name , franchName;

--7--
WITH
    team_20
AS 
(
    SELECT
        teamID, yearID 
    FROM 
        teams
    WHERE
        W >= 20 
)
,
    team_winning_perc
AS
(

    SELECT 
        team_20.teamID AS teamID , name, team_20.yearID, (W::float/G)*100 AS winning_percentage
    FROM
        team_20 , teams
    WHERE
        team_20.teamID = teams.teamID
    AND
        team_20.yearID = teams.yearID
    ORDER BY 
        winning_percentage DESC, team_20.teamID, name , team_20.yearID
)

SELECT 
    team_winning_perc.teamID , team_winning_perc.name , team_winning_perc.yearID, team_winning_perc.winning_percentage
FROM
    team_winning_perc
WHERE
    (team_winning_perc.teamID  , winning_percentage) = ANY(
        SELECT 
            teamID , MAX(winning_percentage)
        FROM 
            team_winning_perc
        GROUP BY
            teamID
    )
LIMIT 5;

--8--
WITH
    max_team_sal 
AS
(
    SELECT
        yearID, teamID, max(salary) AS salary
    FROM
        salaries
    GROUP BY
        yearID, teamID
    ORDER BY 
        yearID , teamID
),
    latest_team_name
AS 
(
    SELECT 
        teamID , name , yearID
    FROM
        teams
    WHERE
        (teamID , yearID) = 
        ANY
        (
            SELECT 
                teamID , MAX(yearID)
            FROM
                teams
            GROUP BY
                teamID
            
        )

    
)
SELECT
    max_team_sal.teamID ,latest_team_name.name as teamname,  max_team_sal.yearID, salaries.playerID, namefirst AS player_first_name , namelast AS player_last_name ,  max_team_sal.salary AS salary
FROM
    max_team_sal , salaries, teams , people, latest_team_name
WHERE
    max_team_sal.yearID = salaries.yearID
AND
    max_team_sal.salary = salaries.salary
AND
    max_team_sal.teamID = salaries.teamID
AND
    teams.teamID = salaries.teamID
AND
    teams.yearID = salaries.yearID
AND
    people.playerID = salaries.playerID
AND
    latest_team_name.teamID = max_team_sal.teamID
ORDER BY
    max_team_sal.teamID, latest_team_name.name,  max_team_sal.yearID, salaries.playerID, player_first_name, player_last_name, salary

--9--
WITH
    pitcher_sal
AS
(
    SELECT 
        AVG(salary) AS avg_sal 
    FROM
        salaries, pitching
    WHERE
        salaries.playerID = pitching.playerID
    AND
        salaries.teamID = pitching.teamID
    AND
        salaries.lgid = pitching.lgid
    AND
        salaries.yearID = pitching.yearID
    AND
        salaries IS NOT NULL
)
,
    batting_sal
AS
(
    SELECT 
        AVG(salary) AS avg_sal
    FROM
        salaries, batting
    WHERE
        salaries.playerID = batting.playerID
    AND
        salaries.teamID = batting.teamID
    AND
        salaries.yearID = batting.yearID

    AND
        salaries.lgid = batting.lgid

    AND
        salary IS NOT NULL

)
SELECT
    CASE
    WHEN
        pitcher_sal.avg_sal > batting_sal.avg_sal
    THEN
        ('pitcher', pitcher_sal.avg_sal)
    ELSE
        ('batsman', batting_sal.avg_sal)
    
    END
FROM
    pitcher_sal , batting_sal

--10--

WITH
    tab1
AS
(
    SELECT * FROM CollegePlaying
)
,
    tab2
AS
(
    SELECT * FROM CollegePlaying
)
,
    tab3
AS
(
    SELECT
        tab1.playerID AS pid1 , tab2.playerID AS pid2
    FROM
        tab1, tab2
    WHERE
        tab1.schoolID = tab2.schoolID
    AND
        tab1.yearID = tab2.yearID
    AND 
        tab1.playerID != tab2.playerID

)
SELECT
    tab3.pid1 AS playerID, namefirst||' '||namelast AS playername , COUNT( DISTINCT tab3.pid2) AS number_of_batchmates
FROM
    tab3 , people
WHERE
    tab3.pid1 = people.playerID
GROUP BY
    tab3.pid1, namefirst, namelast
UNION
SELECT
    CollegePlaying.playerID , namefirst||' '||namelast as playername , 0 as number_of_batchmates
FROM   
    CollegePlaying , people

WHERE
    CollegePlaying.playerID NOT IN (SELECT pid1 from tab3) 
AND
    CollegePlaying.playerID = people.playerID 

ORDER BY
    number_of_batchmates DESC, playerID 

--11--
WITH
    teams_110_wins
AS
(
    SELECT 
        teamID, yearID, G, WSWin
    FROM
        teams
    WHERE
        G >= 110
    AND
        WSWin = true

)
,
    latest_team_name
AS 
(
    SELECT 
        teamID , name , yearID
    FROM
        teams
    WHERE
        (teamID , yearID) = 
        ANY
        (
            SELECT 
                teamID , MAX(yearID)
            FROM
                teams
            GROUP BY
                teamID
            
        )

    
)
SELECT 
    latest_team_name.teamID, name , COUNT(*) AS total_WS_wins
FROM
    latest_team_name , teams_110_wins
WHERE
    latest_team_name.teamID = teams_110_wins.teamID
GROUP BY
    latest_team_name.teamID , name 
ORDER BY
    total_WS_wins DESC , teamID, name
LIMIT 5;

--12--
WITH
    saves_table
AS
(
    SELECT
        playerID, sum(SV) AS saves
    FROM
        pitching
    GROUP BY
        playerID
)
,
    seasons_table
AS
(
    SELECT
        playerID, COUNT( distinct yearID) AS num_seasons
        
    FROM
        pitching
    GROUP BY
        playerID

)
SELECT
    people.playerID ,namefirst AS firstname , namelast AS lastname ,saves , num_seasons
FROM
    people, saves_table, seasons_table
WHERE
    people.playerID = saves_table.playerID
AND
    saves_table.playerID = seasons_table.playerID
AND
    num_seasons >= 15
ORDER BY 
    saves DESC, num_seasons DESC, playerID, firstname, lastname
LIMIT 10;

--13--

WITH
    pitchers_team_ct
AS
(    SELECT 
        playerID , COUNT(DISTINCT teamID) AS num_teams
    FROM
        pitching
    GROUP BY
        playerID
)
,
    pitchers_team_ct_5
AS
(
    SELECT 
        playerID
    FROM
        pitchers_team_ct
    WHERE
        num_teams >= 5
        
)
,
    first_team
AS
(
    SELECT
        pitchers_team_ct_5.playerID  as playerID, teamID , yearID , stint
    FROM
        pitchers_team_ct_5 , pitching
    WHERE
        pitchers_team_ct_5.playerID = pitching.playerID
    AND
        (pitchers_team_ct_5.playerID , yearID ,stint ) = 
        ANY
        (
            SELECT 
                playerID , MIN(yearID)  , MIN(stint)
            FROM
                pitching
            WHERE
                (playerID , yearID  , stint) =
            ANY
            (
                SELECT
                    playerID , yearID , min(stint)
                FROM
                    pitching
                GROUP BY
                    playerID, yearID 
                    
            )

            GROUP BY
                playerID
        )
    ORDER BY
        playerID
)
,
    not_first_team
AS
(
    SELECT
        first_team.playerID, pitching.teamID , pitching.yearID ,first_team.stint
    FROM    
        first_team, pitching
    WHERE
        first_team.playerID = pitching.playerID
    AND
        first_team.teamID != pitching.teamID
    AND
        first_team.stint = pitching.stint
    ORDER BY
        playerID

)   
,
    second_team
AS
(
    SELECT 
        not_first_team.playerID as playerID, teamID , yearID ,stint
    FROM
        not_first_team
    WHERE 
        (not_first_team.playerID  , yearID , stint) = 
        ANY
        (
            SELECT 
                playerID , MIN(yearID) , MIN(stint)
            FROM
                not_first_team
            WHERE
                (playerID , yearID  , stint) =
            ANY
            (
                SELECT
                    playerID , yearID , min(stint)
                FROM
                    not_first_team
                GROUP BY
                    playerID, yearID 
                    
            )
            GROUP BY
                playerID
        )
    ORDER BY
        playerID
)
,
    latest_team_name
AS 
(
    SELECT 
        teamID , name , yearID
    FROM
        teams
    WHERE
        (teamID , yearID) = 
        ANY
        (
            SELECT 
                teamID , MAX(yearID)
            FROM
                teams
            GROUP BY
                teamID
            
        )

    
)
,
    first_team_name
AS
(
    SELECT   
        first_team.playerID , name
    FROM
        first_team , latest_team_name
    WHERE
        first_team.teamID = latest_team_name.teamID
            
)
,
    second_team_name
AS
(

    SELECT   
        second_team.playerID , name
    FROM
        second_team , latest_team_name
    WHERE
        second_team.teamID = latest_team_name.teamID
)
SELECT
    first_team_name.playerID as playerID, namefirst AS firstname , namelast AS lastname,
    birthCity||' '||birthState||' '||birthCountry AS birth_address,
    first_team_name.name AS first_teamname  , second_team_name.name AS second_teamname
FROM
    first_team_name, second_team_name, people
WHERE
    first_team_name.playerID = people.playerID
AND
    second_team_name.playerID = people.playerID
ORDER BY
    playerID, firstname, lastname, birth_address, first_team_name , second_team_name

--14--
--insert queries--
insert into people(playerid , namefirst , namelast ) values ('dunphil02', 'Phil' , 'Dunphy');
insert into people(playerid , namefirst , namelast ) values ('tuckcam01', 'Cameron' , 'Tucker');
insert into people(playerid , namefirst , namelast ) values ('scottm02', 'Michael' , 'Scott');
insert into people(playerid , namefirst , namelast ) values ('waltjoe', 'Joe' , 'Walt');
insert into people(playerid , namefirst , namelast ) values ('adamswi01', 'Willie' , 'Adams');
insert into people(playerid , namefirst , namelast ) values ('yostne01', 'Ned' , 'Yost');

insert into awardsplayers(awardid , playerid , lgid , yearid , tie) values('Best Baseman' , 'dunphil02' , '' , 2014 , true);
insert into awardsplayers(awardid , playerid , lgid , yearid , tie) values('Best Baseman' , 'tuckcam01' , '' , 2014 , true);
insert into awardsplayers(awardid , playerid , lgid , yearid , tie) values('ALCS MVP' , 'scottm02' , 'AA' , 2015 , true);
insert into awardsplayers(awardid , playerid , lgid , yearid , tie) values('Triple Crown' , 'waltjoe' , '' , 2016 , true);
insert into awardsplayers(awardid , playerid , lgid , yearid , tie) values('Gold Glove' , 'adamswi01' , '' , 2017 , true);
insert into awardsplayers(awardid , playerid , lgid , yearid , tie) values('ALCS MVP' , 'yostne01' , '' , 2017 , true);



WITH
    awards_num
AS
(
    SELECT
        awardid , playerID  ,COUNT(playerID) AS num_wins
    FROM
        awardsplayers 
    GROUP BY
        awardid , playerID
    ORDER BY 
        awardid
)
,
    max_awards_num
AS
(
    SELECT 
        awardid , playerID ,  num_wins 
    FROM 
        awards_num
    WHERE
        (awardid , num_wins ) = 
        ANY
        (
            SELECT 
                awardid , max(num_wins)
            FROM
                awards_num
            GROUP BY
                awardid
        )
)
SELECT
    awardid , max_awards_num.playerID , namefirst as firstname , namelast as lastname, num_wins 
FROM
    max_awards_num, people 
WHERE
    max_awards_num.playerID = people.playerID
AND
    (awardid , max_awards_num.playerID )= 
    ANY
    (
        SELECT
            awardid , min(playerID)
        FROM
            max_awards_num
        GROUP BY
            awardid
        
    )
ORDER BY
    awardid , num_wins

--15--
WITH
    managers_2000_2010
AS
(
    SELECT
        playerID, yearID , teamID 
    FROM
        managers
    WHERE
        yearID <= 2010
    AND
        yearID >= 2000
    AND
        (inseason = 0
        OR
        inseason = 1
        )
)
,
    latest_team_name
AS 
(
    SELECT 
        teamID , name , yearID
    FROM
        teams
    WHERE
        (teamID , yearID) = 
        ANY
        (
            SELECT 
                teamID , MAX(yearID)
            FROM
                teams
            GROUP BY
                teamID
            
        )

    
)
SELECT
    managers_2000_2010.teamID as teamID, name as teamname , managers_2000_2010.yearID  as seasonid, managers_2000_2010.playerID as managerid,
    namefirst as managerfirstname, namelast as managerlastname 
FROM
    people , managers_2000_2010 , latest_team_name
WHERE
    people.playerID = managers_2000_2010.playerID 
AND
    latest_team_name.teamID  = managers_2000_2010.teamID 
ORDER BY
    teamID , teamname, seasonid DESC , managerid ,managerfirstname, managerlastname 

--16--
WITH
    num_awards 
AS
(
    SELECT 
        playerID , COUNT(awardid) AS total_awards
    FROM
        awardsplayers
    GROUP BY
        playerID
    ORDER BY 
        total_awards DESC , playerid
    LIMIT 10

)
,
    last_college
AS
(
    SELECT
        playerID , schoolID 
    FROM
        CollegePlaying
    WHERE
        (playerID  , yearID) = 
        ANY
        (
            SELECT
                playerID , max(yearID)
            FROM
                CollegePlaying
            GROUP BY
                playerID
        ) 
)
SELECT
    num_awards.playerID,  schoolName as College_name , total_awards 
FROM
    num_awards  
LEFT JOIN
    last_college
ON
    num_awards.playerID = last_college.playerID
LEFT JOIN
    schools
ON
    schools.schoolID = last_college.schoolID
ORDER BY
    total_awards DESC , College_name , playerid

--17--
WITH
    first_player_award
AS
(
    SELECT 
        playerID , awardid , yearID
    FROM
        awardsplayers
    WHERE
        (yearID , playerID) =
        ANY
        (
            SELECT 
                min(yearID) , playerID 
            FROM
                awardsplayers
            GROUP BY
                playerID
        )
)
,
    first_alpha_player_award
AS
(
    SELECT DISTINCT
        playerID , awardid, yearID
    FROM
        first_player_award
    WHERE
        (awardid , playerID) = 
        ANY
        (
            SELECT 
                min(awardid) , playerID
            FROM
                first_player_award
            GROUP BY
                playerID
        )
    ORDER BY 
        playerID

)
,
    first_manager_award
AS
(
    SELECT 
        playerID , awardid , yearID
    FROM
        awardsmanagers
    WHERE
        (yearID , playerID) =
        ANY
        (
            SELECT 
                min(yearID) , playerID 
            FROM
                awardsmanagers
            GROUP BY
                playerID
        )
)
,
    first_alpha_manager_award
AS
(
    SELECT DISTINCT
        playerID , awardid, yearID
    FROM
        first_manager_award
    WHERE
        (awardid , playerID) = 
        ANY
        (
            SELECT 
                min(awardid) , playerID
            FROM
                first_manager_award
            GROUP BY
                playerID
        )
    ORDER BY 
        playerID

)

SELECT 
    people.playerID as playerID, namefirst as firstname, namelast as lastname , first_alpha_player_award.awardid as playerawardid,
    first_alpha_player_award.yearID as playerawardyear, first_alpha_manager_award.awardid as managerawardid ,
    first_alpha_manager_award.yearID as managerawardyear
FROM
    people , first_alpha_manager_award , first_alpha_player_award 
WHERE
    first_alpha_manager_award.playerID = first_alpha_player_award.playerID
AND
    first_alpha_manager_award.playerID = people.playerID
ORDER BY
    playerID, firstname, lastname

--18--
WITH
    halloffame_ct
AS
(
    SELECT 
        playerID, COUNT(DISTINCT category) AS num_honored_categories 
    FROM
        halloffame
    GROUP BY
        playerID

)
,
    halloffame_ct_2
AS
(
    SELECT
        playerID, num_honored_categories
    FROM
        halloffame_ct
    WHERE
        num_honored_categories >= 2
)
,
    allstarfirstyear
AS
(
    SELECT
        playerID , MIN(yearID) as yearID
    FROM
        AllstarFull
    GROUP BY
        playerID
    ORDER BY
        playerID
)

SELECT
    halloffame_ct_2.playerID, namefirst as firstname, namelast as lastname, num_honored_categories, yearID
FROM
    halloffame_ct_2 , people , allstarfirstyear
WHERE
    halloffame_ct_2.playerID = allstarfirstyear.playerID
AND
    halloffame_ct_2.playerID = people.playerID
ORDER BY
    num_honored_categories DESC, playerID , firstname , lastname, yearID

--19--
WITH
    combineplayers
AS
(
    SELECT
        playerID, sum(g_1b)  as G_1b, sum(g_2b) as G_2b, sum(g_3b) as G_3b, sum(g_all) as G_all
    FROM
        appearances
    GROUP BY
        playerID
)

SELECT
    combineplayers.playerID, namefirst as firstname  , namelast as lastname , G_all , G_1b , g_2b, g_3b 
FROM
    combineplayers, people
WHERE
    combineplayers.playerID = people.playerID 
AND
(
    (
        g_1b > 0
    AND
        g_2b > 0 
    )
    OR
    (
        g_2b > 0 
    AND
        g_3b > 0 
    )
    OR
    (
        g_3b > 0 
    AND
        g_1b > 0 
    )

)
ORDER BY
    g_all DESC , playerID , firstname  ,lastname , g_1b DESC, g_2b DESC, g_3b DESC

--20--
WITH
    school_ct
AS
(
    SELECT 
        schoolID , COUNT(DISTINCT playerID ) as student_count
    FROM
        CollegePlaying
    GROUP BY
        schoolID
    ORDER BY
        student_count DESC
    LIMIT 5 
    
)

SELECT DISTINCT
    school_ct.schoolID  , schoolName, schoolcity||' '||schoolstate  as schooladdr , people.playerID , namefirst as firstname ,namelast as lastname
FROM
    people , school_ct , schools , CollegePlaying
WHERE
    people.playerID = CollegePlaying.playerID
AND
    school_ct.schoolID = schools.schoolID
AND
    school_ct.schoolID = CollegePlaying.schoolID
ORDER BY 
    schoolID, schoolName, schooladdr, people.playerID, firstname , lastname
    
--21--
WITH
    same_birthplace
AS
(
    SELECT
        a.playerID as playerID_1 , b.playerID as playerID_2 , a.birthCity , a.birthState
    FROM
        people a , people b 
    WHERE
        a.birthState = b.birthState
    AND
        a.birthCity = b.birthCity    
    AND
        a.playerID != b.playerID
    ORDER BY
        a.playerID
)
,
    batting_teams
AS
(
    SELECT DISTINCT
        people.playerID , teamID , birthCity , birthState
    FROM
        batting, people
    WHERE
        batting.playerID = people.playerID
)
,
    samebatting_birth
AS
(

    SELECT DISTINCT
        a.playerID as playerID_1 , b.playerID as playerID_2 , a.birthCity , b.birthState
    FROM
        batting_teams a , batting_teams b 
    WHERE
        a.teamID = b.teamID
    AND
        a.birthCity = b.birthCity
    AND
        a.birthState = b.birthState
    AND
        a.playerID != b.playerID

)
,
    pitching_teams
AS
(
    SELECT DISTINCT
        people.playerID , teamID , birthCity , birthState
    FROM
        pitching, people
    WHERE
        pitching.playerID = people.playerID

)
,
    samepitching_birth
AS
(
    SELECT DISTINCT
        a.playerID as playerID_1 , b.playerID as playerID_2 , a.birthCity , b.birthState
    FROM
        pitching_teams a , pitching_teams b 
    WHERE
        a.teamID = b.teamID
    AND
        a.birthCity = b.birthCity
    AND
        a.birthState = b.birthState
    AND
        a.playerID != b.playerID
    
)
,
    samepitchingbatting_birth
AS
(
    SELECT 
        playerID_1 , playerID_2 , birthCity , birthState
    FROM
        samebatting_birth
    INTERSECT
    SELECT 
        playerID_1 , playerID_2 , birthCity , birthState
    FROM
        samepitching_birth

)
,
    onlybatting_birth
AS
(
    SELECT * FROM 
        samebatting_birth
    EXCEPT
    
    SELECT * FROM
        samepitchingbatting_birth
)
,
    onlypitching_birth
AS
(
    SELECT * FROM
        samepitching_birth
    EXCEPT
    SELECT * FROM
        samepitchingbatting_birth
)
SELECT
    playerID_1 , playerID_2 ,birthCity , birthState ,'batted' as role 
FROM
    onlybatting_birth
UNION
SELECT
    playerID_1 , playerID_2 ,birthCity , birthState ,'pitched' as role 
FROM
    onlypitching_birth
UNION
SELECT
    playerID_1 , playerID_2 ,birthCity , birthState ,'both' as role 
FROM
    samepitchingbatting_birth
ORDER BY
    birthCity , birthState, playerID_1, playerID_2

--22--
WITH
    award_avg_pts
AS
(
    SELECT
        awardid, yearID , AVG(pointsWon) AS averagepoints
    FROM
        AwardsSharePlayers
    GROUP BY
        awardid , yearID
)

SELECT 
    award_avg_pts.awardid, award_avg_pts.yearID as seasonid, playerID, pointsWon as playerpoints ,  averagepoints
FROM
    award_avg_pts , AwardsSharePlayers
WHERE
    award_avg_pts.awardid = AwardsSharePlayers.awardid
AND
    award_avg_pts.yearID = AwardsSharePlayers.yearID
AND
    pointsWon >= averagepoints
ORDER BY
    awardid , seasonid, playerpoints DESC,  playerID
    
--23--
WITH
    no_award
AS
(
    SELECT 
        playerID
    FROM
        people
    EXCEPT
    (
        SELECT DISTINCT
            playerID
        FROM
            awardsplayers
        UNION
        SELECT DISTINCT
            playerID
        FROM
            awardsmanagers
    )


)
SELECT
    no_award.playerID, namefirst||' '||namelast as playername , 
    CASE
    WHEN
        deathyear IS NULL
    THEN
        true
    ELSE
        false
    END
    AS alive
FROM
    people, no_award
WHERE
    people.playerID = no_award.playerID
ORDER BY
    playerID, playername

--24--
WITH recursive
    graph_1_helper_1
AS
(
    SELECT 
        playerID, yearID, teamID
    FROM
        pitching
    UNION
    SELECT
        playerID, yearID , teamID
    FROM
        AllstarFull
    WHERE
        GP = 1
    
),

    players_g1h
AS
(
    SELECT DISTINCT
       playerID
    FROM
    graph_1_helper_1

)    
,
    serial_num_players
AS
(

    SELECT 
        ROW_NUMBER() over(ORDER BY playerID) as serial_num, playerID
    FROM
        players_g1h

)
,
    graph_1_helper_2
AS
(
    SELECT
        a.playerID as playerID_1 , b.playerID as playerID_2, a.teamID , a.yearID
    FROM
        graph_1_helper_1 a , graph_1_helper_1  b
    WHERE
        a.teamID = b.teamID
    AND
        a.yearID = b.yearID
    AND
        a.playerID != b.playerID
        
)
,
    graph_1_helper_3
AS
(
    SELECT
        playerID_1 , playerID_2 , teamID , count(yearID) as gamesplayedtogather
    FROM
        graph_1_helper_2
    GROUP BY
        playerID_1 , playerID_2 , teamID

)
,
    graph_1
AS
(
    SELECT
        playerID_1 , playerID_2 , sum(gamesplayedtogather) weight
    FROM
        graph_1_helper_3
    GROUP BY
        playerID_1 , playerID_2
    ORDER BY
        weight DESC , playerID_1 , playerID_2
)
,
     len_3_search
AS
(
    SELECT 
        playerID_1 , playerID_2 ,array[ser1.serial_num , ser2.serial_num ] as path1 ,  weight as length  , 1 as depth
    FROM
        graph_1 , serial_num_players ser1 , serial_num_players ser2
    WHERE
        playerID_1 = 'webbbr01'
    AND
        ser1.playerID  = playerID_1
    AND
        ser2.playerID = playerID_2
    
    UNION ALL
    
    SELECT DISTINCT
        H.playerID_2 as playerID_1, E.playerID_2 as playerID_2 ,H.path1||ser1.serial_num  ,  H.length + E.weight as length, depth+1
    FROM
        len_3_search H , serial_num_players ser1 , graph_1 E , serial_num_players ser2 , serial_num_players ser3 
    
    WHERE
        H.playerID_2 = E.playerID_1
    

    AND   
        NOT ser1.serial_num  = ANY(path1)
        
        
    AND
        ser1.playerID = E.playerID_2
    AND
        ser2.playerID = H.playerID_2
    AND
        ser3.playerID = 'clemero02' 
    AND
        depth < 3

)
SELECT 
    CASE
    WHEN
        'clemero02' IN (SELECT(playerID_2) FROM len_3_search WHERE length >= 3)
    THEN
        true
    else 
        false
    END
    AS
    pathexists
    
--25--
WITH recursive
    graph_1_helper_1
AS
(
    SELECT 
        playerID, yearID, teamID
    FROM
        pitching
    UNION
    SELECT
        playerID, yearID , teamID
    FROM
        AllstarFull
    WHERE
        GP = 1
    
),

    players_g1h
AS
(
    SELECT DISTINCT
       playerID
    FROM
    graph_1_helper_1

)    
,
    serial_num_players
AS
(

    SELECT 
        ROW_NUMBER() over(ORDER BY playerID) as serial_num, playerID
    FROM
        players_g1h

)
,
    graph_1_helper_2
AS
(
    SELECT
        a.playerID as playerID_1 , b.playerID as playerID_2, a.teamID , a.yearID
    FROM
        graph_1_helper_1 a , graph_1_helper_1  b
    WHERE
        a.teamID = b.teamID
    AND
        a.yearID = b.yearID
    AND
        a.playerID != b.playerID
        
)
,
    graph_1_helper_3
AS
(
    SELECT
        playerID_1 , playerID_2 , teamID , count(yearID) as gamesplayedtogather
    FROM
        graph_1_helper_2
    GROUP BY
        playerID_1 , playerID_2 , teamID

)
,
    graph_1
AS
(
    SELECT
        playerID_1 , playerID_2 , sum(gamesplayedtogather) weight
    FROM
        graph_1_helper_3
    GROUP BY
        playerID_1 , playerID_2
    ORDER BY
        weight DESC , playerID_1 , playerID_2
    
)

,
    pathsfrom
AS
(
    SELECT 
        playerID_1 , playerID_2 ,array[ser1.serial_num , ser2.serial_num ] as path1 ,  weight as length  , 1 as depth
    FROM
        graph_1 , serial_num_players ser1 , serial_num_players ser2
    WHERE
        playerID_1 = 'garcifr02'
    AND
        ser1.playerID  = playerID_1
    AND
        ser2.playerID = playerID_2
    
    UNION ALL
    
    SELECT DISTINCT
        H.playerID_2 as playerID_1, E.playerID_2 as playerID_2 ,H.path1||ser1.serial_num  ,  H.length + E.weight as length, depth+1
    FROM
        pathsfrom H , serial_num_players ser1 , graph_1 E , serial_num_players ser2 , serial_num_players ser3 
    
    WHERE
        H.playerID_2 = E.playerID_1
    

    AND   
        NOT ser1.serial_num  = ANY(path1)
    AND
        NOT ser3.serial_num  = ANY(path1) 
        
    AND
        ser1.playerID = E.playerID_2
    AND
        ser2.playerID = H.playerID_2
    AND
        ser3.playerID = 'leagubr01' 
    
)
,
    pthsreq
AS
(
    SELECT
        min(length) as pathlength
    FROM
        pathsfrom
    WHERE
        playerID_2 = 'leagubr01'
)
SELECT * FROM pthsreq

--26--
WITH recursive
    seriespostg
AS
(
    SELECT 
        teamIDwinner , teamIDloser 
    FROM
        SeriesPost
)
,
    allplayers
AS
(
    SELECT
        teamIDwinner AS teamID
    FROM
        SeriesPostg
    UNION
    SELECT
        teamIDloser AS teamID
    FROM
        seriespostg

)
,
    team_num
AS
(
    SELECT 
        ROW_NUMBER()  over(ORDER BY teamID)  as serial_num  , teamID
    FROM
        allplayers

)
,
    bfs1
AS
(
    SELECT
        teamIDwinner , teamIDloser , array[ser1.serial_num , ser2.serial_num] as path1, 1 as depth 
    FROM
        seriespostg , team_num ser1 , team_num ser2 
    WHERE
        teamIDwinner = 'ARI'
    AND
        teamIDwinner = ser1.teamID
    AND
        teamIDloser = ser2.teamID
    UNION ALL
    SELECT DISTINCT
        H.teamIDloser as teamIDwinner , E.teamIDloser as teamIDloser , H.path1||ser1.serial_num , depth+1
    FROM
        bfs1 H , seriespostg E , team_num ser1 ,  team_num ser3
    WHERE
        H.teamIDloser = E.teamIDwinner
    AND
        ser1.teamID = E.teamIDloser
    AND
        ser3.teamID = 'DET'
    AND
        NOT ser3.serial_num = ANY(H.path1) 
    AND
        NOT ser1.serial_num = ANY(H.path1)
      

)


SELECT 
    COUNT(DISTINCT path1) 
FROM 
    bfs1, team_num
WHERE
    team_num.serial_num = ANY(path1)
AND
    team_num.teamID = 'DET'

--27--
WITH recursive
    seriespostg
AS
(
    SELECT 
        teamIDwinner , teamIDloser 
    FROM
        SeriesPost
)
,
    allplayers
AS
(
    SELECT
        teamIDwinner AS teamID
    FROM
        SeriesPostg
    UNION
    SELECT
        teamIDloser AS teamID
    FROM
        seriespostg

)
,
    team_num
AS
(
    SELECT 
        ROW_NUMBER()  over(ORDER BY teamID)  as serial_num  , teamID
    FROM
        allplayers

)
,
    bfs1
AS
(
    SELECT
        teamIDwinner , teamIDloser , array[ser1.serial_num , ser2.serial_num] as path1, 1 as depth 
    FROM
        seriespostg , team_num ser1 , team_num ser2 
    WHERE
        teamIDwinner = 'HOU'
    AND
        teamIDwinner = ser1.teamID
    AND
        teamIDloser = ser2.teamID
    UNION ALL
    SELECT DISTINCT
        H.teamIDloser as teamIDwinner , E.teamIDloser as teamIDloser , H.path1||ser1.serial_num , depth+1
    FROM
        bfs1 H , seriespostg E , team_num ser1 
    WHERE
        H.teamIDloser = E.teamIDwinner
    AND
        ser1.teamID = E.teamIDloser
    AND
        NOT ser1.serial_num = ANY(H.path1)
    AND
        depth < 3
    
    

)


SELECT 
    teamIDloser as teamID, max(depth) as num_hops
FROM
    bfs1
WHERE
    teamIDloser != 'HOU'
GROUP BY
    teamIDloser
ORDER BY
    teamID

--28--

WITH recursive
    seriespostg
AS
(
    SELECT 
        teamIDwinner , teamIDloser 
    FROM
        SeriesPost
)
,
    allplayers
AS
(
    SELECT
        teamIDwinner AS teamID
    FROM
        SeriesPostg
    UNION
    SELECT
        teamIDloser AS teamID
    FROM
        seriespostg

)
,
    team_num
AS
(
    SELECT 
        ROW_NUMBER()  over(ORDER BY teamID)  as serial_num  , teamID
    FROM
        allplayers

)
,
    bfs1
AS
(
    SELECT
        teamIDwinner , teamIDloser , array[ser1.serial_num , ser2.serial_num] as path1, 1 as depth 
    FROM
        seriespostg , team_num ser1 , team_num ser2 
    WHERE
        teamIDwinner = 'WS1'
    AND
        teamIDwinner = ser1.teamID
    AND
        teamIDloser = ser2.teamID
    UNION ALL
    SELECT DISTINCT
        H.teamIDloser as teamIDwinner , E.teamIDloser as teamIDloser , H.path1||ser1.serial_num , depth+1
    FROM
        bfs1 H , seriespostg E , team_num ser1 
    WHERE
        H.teamIDloser = E.teamIDwinner
    AND
        ser1.teamID = E.teamIDloser
    AND
        NOT ser1.serial_num = ANY(H.path1)
    
    
    

),

    latest_team_name
AS 
(
    SELECT 
        teamID , name , yearID
    FROM
        teams
    WHERE
        (teamID , yearID) = 
        ANY
        (
            SELECT 
                teamID , MAX(yearID)
            FROM
                teams
            GROUP BY
                teamID
            
        )

    
)

SELECT DISTINCT
    teamIDloser as teamID, name as teamname ,  depth as pathlength 
FROM 
    bfs1 , latest_team_name
WHERE 
    (teamID , depth) = 
        ANY
        (
            SELECT 
                teamID , MAX(depth)
            FROM
                bfs1
            GROUP BY
                teamID
        )
AND
    teamIDloser = teamID
ORDER BY
    teamID , teamname

--29--
WITH  recursive
    latest_team_name
AS 
(
    SELECT 
        teamID , name , yearID
    FROM
        teams
    WHERE
        (teamID , yearID) = 
        ANY
        (
            SELECT 
                teamID , MAX(yearID)
            FROM
                teams
            GROUP BY
                teamID
            
        )

    
),

    ties_more_than_loss
AS
(
    SELECT DISTINCT
        seriespost.teamIDwinner as teamID, latest_team_name.name as teamname 
    FROM
        SeriesPost , latest_team_name
    WHERE
        ties > losses
    AND
        seriespost.teamIDwinner = latest_team_name.teamID


)
,
    seriespostg
AS
(
    SELECT 
        teamIDwinner , teamIDloser 
    FROM
        SeriesPost
),

    allplayers
AS
(
    SELECT
        teamIDwinner AS teamID
    FROM
        SeriesPostg
    UNION
    SELECT
        teamIDloser AS teamID
    FROM
        seriespostg

)
,

    team_num
AS
(
    SELECT 
        ROW_NUMBER()  over(ORDER BY teamID)  as serial_num  , teamID
    FROM
        allplayers

)


,
    bfs1
AS
(
    SELECT
        teamIDwinner , teamIDloser , array[ser1.serial_num , ser2.serial_num] as path1, 1 as depth 
    FROM
        seriespostg , team_num ser1 , team_num ser2 , ties_more_than_loss
    WHERE
        teamIDwinner = ties_more_than_loss.teamID
    AND
        teamIDwinner = ser1.teamID
    AND
        teamIDloser = ser2.teamID
    UNION ALL
    SELECT DISTINCT
        H.teamIDloser as teamIDwinner , E.teamIDloser as teamIDloser , H.path1||ser1.serial_num , depth+1
    FROM
        bfs1 H , seriespostg E , team_num ser1 
    WHERE
        H.teamIDloser = E.teamIDwinner
    AND
        ser1.teamID = E.teamIDloser
    AND
        NOT ser1.serial_num = ANY(H.path1)
    
   
    

)
SELECT 
    ties_more_than_loss.teamID as teamID, min(depth) as pathlength 
FROM
    bfs1 , ties_more_than_loss , team_num ser1
WHERE
    ser1.teamID = ties_more_than_loss.teamID
AND
    path1[1] = ser1.serial_num
AND
    teamIDloser = 'NYA'
GROUP BY
    ties_more_than_loss.teamID
ORDER BY
    teamID, pathlength

--30--
WITH  recursive
    latest_team_name
AS 
(
    SELECT 
        teamID , name , yearID
    FROM
        teams
    WHERE
        (teamID , yearID) = 
        ANY
        (
            SELECT 
                teamID , MAX(yearID)
            FROM
                teams
            GROUP BY
                teamID
            
        )

    
)
,
    seriespostg
AS
(
    SELECT 
        teamIDwinner , teamIDloser 
    FROM
        SeriesPost
),

    allplayers
AS
(
    SELECT
        teamIDwinner AS teamID
    FROM
        SeriesPostg
    UNION
    SELECT
        teamIDloser AS teamID
    FROM
        seriespostg

)
,

    team_num
AS
(
    SELECT 
        ROW_NUMBER()  over(ORDER BY teamID)  as serial_num  , teamID
    FROM
        allplayers

)


,
    bfs1
AS
(
    SELECT
        teamIDwinner , teamIDloser , array[ser1.serial_num , ser2.serial_num] as path1, 1 as depth 
    FROM
        seriespostg , team_num ser1 , team_num ser2 
    WHERE
        teamIDwinner = 'DET'
    AND
        teamIDwinner = ser1.teamID
    AND
        teamIDloser = ser2.teamID
    UNION ALL
    SELECT DISTINCT
        H.teamIDloser as teamIDwinner , E.teamIDloser as teamIDloser , H.path1||ser1.serial_num , depth+1
    FROM
        bfs1 H , seriespostg E , team_num ser1 
    WHERE
        H.teamIDloser = E.teamIDwinner
    AND
        ser1.teamID = E.teamIDloser
    AND
        (
                NOT ser1.serial_num = ANY(H.path1)
            OR
                E.teamIDloser = 'DET'

        )
      

)
,
    path1_ct
AS
(
    SELECT 
        depth as cyclelength , path1
    FROM
        bfs1 , team_num
    WHERE
        team_num.teamID = 'DET'
    AND
        path1[1] = team_num.serial_num
    AND
        path1[depth + 1] = team_num.serial_num

)
SELECT 
    cyclelength , count(path1)
FROM
    path1_ct
WHERE
    cyclelength = (SELECT max(cyclelength) FROM path1_ct)
GROUP BY
    cyclelength




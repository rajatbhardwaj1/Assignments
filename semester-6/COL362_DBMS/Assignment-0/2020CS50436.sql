SELECT tournament_id , tournament_name , year , winner  from tournaments where tournaments.host_country = tournaments.winner ;

SELECT player_id , family_name , given_name , count_tournaments from players where count_tournaments >= 4 ;

SELECT count(*) from matches join teams on (matches.home_team_id = teams.team_id or teams.team_id = matches.away_team_id) and teams.team_name = 'Croatia' and matches.draw = true ;

SELECT DISTINCT stadium_name , city_name ,country_name from stadiums join matches on (stadiums.stadium_id = matches.stadium_id) join tournaments on matches.tournament_id  = tournaments.tournament_id and matches.stage_name = 'final' and tournaments.tournament_name = '1990 FIFA World Cup' ;

SELECT COUNT(*) FROM goals JOIN players ON (players.player_id = goals.player_id)  AND players.family_name = 'Ronaldo' AND players.given_name = 'Cristiano' and goals.own_goal = false  ;

SELECT goals.player_id , players.family_name , players.given_name , count(goals.player_id) as t from goals inner join players on players.player_id = goals.player_id where own_goal = false group by players.player_id , goals.player_id , players.family_name , players.given_name  having count(goals.player_id) = (select max(c) from (select count(goals.player_id) as c from goals group by goals.player_id , own_goal) as q) ; 

SELECT tab.player_team_id , tab.team_name, max(t) from(SELECT goals.player_team_id , teams.team_name , count(goals.player_team_id) as t from goals join teams on teams.team_id = goals.player_team_id join matches on matches.match_id = goals.match_id  join tournaments on tournaments.tournament_id = matches.tournament_id where own_goal=true and tournaments.year >= 2010 group by teams.team_id, goals.player_team_id, teams.team_name order by player_team_id) as tab where tab.t = (select max(t) from(SELECT goals.player_team_id , teams.team_name , count(goals.player_team_id) as t from goals join teams on teams.team_id = goals.player_team_id join matches on matches.match_id = goals.match_id  join tournaments on tournaments.tournament_id = matches.tournament_id where own_goal=true and tournaments.year >= 2010 group by teams.team_id, goals.player_team_id, teams.team_name order by player_team_id) as q)  group by tab.player_team_id,tab.team_name ;


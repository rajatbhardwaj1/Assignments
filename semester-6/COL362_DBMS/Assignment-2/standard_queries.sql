--1--
WITH personpairs AS
(
SELECT t1.id AS person1sid, t2.id AS person2sid
FROM person AS t1
JOIN person AS t2
ON t1.id < t2.id
EXCEPT
SELECT person1id AS person1sid, person2id AS person2sid
FROM person_knows_person
),
addingtags AS
(
SELECT personpairs.person1sid, personpairs.person2sid, t1.tagid
FROM personpairs
JOIN person_hasinterest_tag AS t1
ON t1.personid = personpairs.person1sid
JOIN person_hasinterest_tag AS t2
ON t2.personid = personpairs.person2sid AND t1.tagid = t2.tagid
JOIN tag ON t1.tagid = tag.id
WHERE :taglist ::text LIKE '%' || tag.name || '%' ::text 
),
counttagdupremove AS
(
SELECT person1sid, person2sid, SUM(tagcount)
FROM
(SELECT person1sid, person2sid, count(DISTINCT tagid) AS tagcount
FROM addingtags
GROUP BY person1sid, person2sid
UNION 
SELECT person1sid, person2sid, 0 AS tagcount
FROM personpairs) AS counttag
GROUP BY person1sid, person2sid
HAVING SUM(tagcount) >= :K
),
addingfriends AS
(
SELECT counttagdupremove.person1sid, counttagdupremove.person2sid, t1.person2id AS friend
FROM counttagdupremove
JOIN person_knows_person AS t1
ON t1.person1id = counttagdupremove.person1sid 
JOIN person_knows_person AS t2
ON t2.person1id = counttagdupremove.person2sid AND t1.person2id = t2.person2id
UNION
SELECT counttagdupremove.person1sid, counttagdupremove.person2sid, t1.person2id AS friend
FROM counttagdupremove
JOIN person_knows_person AS t1
ON t1.person1id = counttagdupremove.person1sid 
JOIN person_knows_person AS t2
ON t2.person2id = counttagdupremove.person2sid AND t1.person2id = t2.person1id
UNION
SELECT counttagdupremove.person1sid, counttagdupremove.person2sid, t1.person1id AS friend
FROM counttagdupremove
JOIN person_knows_person AS t1
ON t1.person2id = counttagdupremove.person1sid 
JOIN person_knows_person AS t2
ON t2.person1id = counttagdupremove.person2sid AND t1.person1id = t2.person2id
UNION
SELECT counttagdupremove.person1sid, counttagdupremove.person2sid, t1.person1id AS friend
FROM counttagdupremove
JOIN person_knows_person AS t1
ON t1.person2id = counttagdupremove.person1sid 
JOIN person_knows_person AS t2
ON t2.person2id = counttagdupremove.person2sid AND t1.person1id = t2.person1id
),
countingmutualfriends AS
(
SELECT person1sid, person2sid, COUNT(friend) AS mutualfriendcount
FROM addingfriends
GROUP BY person1sid, person2sid
),
addinglikedmessages AS
(
SELECT addingfriends.person1sid, addingfriends.person2sid, post.id as messageid
FROM addingfriends
JOIN post ON post.creatorpersonid = addingfriends.friend
JOIN person_likes_post AS t1 ON t1.personid = addingfriends.person1sid AND t1.postid = post.id
JOIN person_likes_post AS t2 ON t2.personid = addingfriends.person2sid AND t2.postid = post.id
WHERE post.creationdate < :lastdate
UNION
SELECT addingfriends.person1sid, addingfriends.person2sid, comment.id as messageid
FROM addingfriends
JOIN comment ON comment.creatorpersonid = addingfriends.friend
JOIN person_likes_comment AS t1 ON t1.personid = addingfriends.person1sid AND t1.commentid = comment.id
JOIN person_likes_comment AS t2 ON t2.personid = addingfriends.person2sid AND t2.commentid = comment.id
WHERE comment.length > :commentlength
),
countmessagedupremove AS
(
SELECT person1sid, person2sid, SUM(messagecount)
FROM 
(
SELECT person1sid, person2sid, COUNT(messageid) AS messagecount
FROM addinglikedmessages
GROUP BY person1sid, person2sid
UNION
SELECT person1sid, person2sid, 0 AS messagecount
FROM counttagdupremove 
) AS countlikedmessages
GROUP BY person1sid, person2sid
HAVING SUM(messagecount) >= :X
)
SELECT countmessagedupremove.person1sid, countmessagedupremove.person2sid, countingmutualfriends.mutualfriendcount
FROM countmessagedupremove
JOIN countingmutualfriends
ON countmessagedupremove.person1sid = countingmutualfriends.person1sid AND countmessagedupremove.person2sid = countingmutualfriends.person2sid
ORDER BY person1sid, mutualfriendcount DESC, person2sid;

--2--
WITH country AS 
(
SELECT id AS country_id
FROM place
WHERE name = 'China'
),
_person as (
select * from person   
),
accepted_cities AS (
SELECT id AS cityid
FROM place
JOIN country ON country.country_id = place.partofplaceid
),
shortlisted_person AS (
SELECT _person.id AS personid, person_studyat_university.universityid AS universityid, EXTRACT(MONTH FROM _person.birthday) AS birth_month
FROM _person , person_studyat_university, accepted_cities
WHERE person_studyat_university.personid = _person.id and 
_person.locationcityid = accepted_cities.cityid and 
_person.creationdate < '2012-07-01'
and _person.creationdate > '2010-06-01'
),
personpairs AS (
SELECT DISTINCT t1.personid AS person1id, t2.personid AS person2id
FROM shortlisted_Person AS t1
JOIN shortlisted_Person AS t2 ON t1.universityid = t2.universityid
AND t1.birth_month = t2.birth_month
AND t1.personid != t2.personid
),
monthanduniversityfilter AS (
SELECT personpairs.person1id, personpairs.person2id
FROM personpairs
JOIN person_knows_person 
ON (personpairs.person1id = person_knows_person.person1id AND personpairs.person2id = person_knows_person.person2id)
OR (personpairs.person2id = person_knows_person.person1id AND personpairs.person1id = person_knows_person.person2id)
),
p2p3table AS (
SELECT t1.person1id AS person1, t1.person2id AS person2, t2.person2id AS person3
FROM monthanduniversityfilter AS t1
JOIN monthanduniversityfilter AS t2 ON t1.person2id = t2.person1id
and t1.person1id != t2.person2id
UNION
SELECT t1.person1id AS person1, t1.person2id AS person2, t2.person1id AS person3
FROM monthanduniversityfilter AS t1
JOIN monthanduniversityfilter AS t2 ON t1.person2id = t2.person2id
and t1.person1id != t2.person1id
),
tripletable AS (
SELECT p2p3table.person1, p2p3table.person2, p2p3table.person3
FROM p2p3table
JOIN person_knows_person 
ON (p2p3table.person1 = person_knows_person.person1id AND p2p3table.person3 = person2id)
OR (p2p3table.person1 = person_knows_person.person2id AND p2p3table.person3 = person1id)
),
triples AS (
SELECT
CASE
    WHEN person1 <= person2
    and person2 <= person3 THEN ARRAY [person1, person2, person3]
    WHEN person1 <= person3
    and person3 <= person2 THEN ARRAY [person1, person3, person2]
    WHEN person2 <= person1
    and person1 <= person3 THEN ARRAY [person2, person1, person3]
    WHEN person2 <= person3
    and person3 <= person1 THEN ARRAY [person2, person3, person1]
    WHEN person3 <= person2
    and person2 <= person1 THEN ARRAY [person3, person2, person1]
    ELSE ARRAY [person3, person1, person2]
END AS triplets
FROM tripletable
)
SELECT COUNT(DISTINCT triplets)
FROM triples;

--3--
WITH messages AS
(
SELECT creationdate, tagid
FROM Comment_hasTag_Tag
UNION ALL
SELECT creationDate, tagid
FROM Post_hasTag_Tag
),
message_create_mid AS
(
SELECT tagid, COUNT(tagid) AS ct
FROM messages
WHERE creationDate >= :begindate
AND creationDate <= :middate
GROUP BY tagid
),
message_mid_end AS
(
SELECT tagid, COUNT(tagid) AS ct
FROM messages
WHERE creationDate >= :middate
AND creationDate <= :enddate
GROUP BY tagid
),
req_tags AS
(
SELECT t1.tagid
FROM message_create_mid t1 , message_mid_end t2 
WHERE t1.tagid = t2.tagid
AND t1.ct >= 5 * t2.ct
),
tag_class_id AS
(
SELECT typetagclassid, COUNT(typetagclassid) AS ct
FROM tag, req_tags
WHERE tag.id = req_tags.tagid 
GROUP BY typetagclassid
) 
SELECT name as tagclassname, ct as count
FROM tag_class_id, TagClass
WHERE tag_class_id.typetagclassid = TagClass.id
ORDER BY count DESC, tagclassname;

--4--
WITH 
messagetags AS
(
SELECT post_hastag_tag.postid AS messages, tag.name AS tagname, COUNT(t1.id) AS num_reply
FROM post_hastag_tag
JOIN post ON post.id = post_hastag_tag.postid
JOIN tag ON post_hastag_tag.tagid = tag.id
JOIN comment AS t1 ON post_hastag_tag.postid = t1.parentpostid
GROUP BY post_hastag_tag.postid, tag.name
HAVING COUNT(t1.id) >= :X
UNION ALL
SELECT comment_hastag_tag.commentid AS messages, tag.name AS tagname, COUNT(t1.id) AS num_reply
FROM comment_hastag_tag
JOIN comment AS c1 ON c1.id = comment_hastag_tag.commentid
JOIN tag ON comment_hastag_tag.tagid = tag.id
JOIN comment AS t1 ON comment_hastag_tag.commentid = t1.parentcommentid
GROUP BY comment_hastag_tag.commentid, tag.name
HAVING COUNT(t1.id) >= :X
),
handle0 AS
(SELECT post_hastag_tag.postid AS messages, tag.name AS tagname, 0 AS num_reply
FROM post_hastag_tag
JOIN tag ON post_hastag_tag.tagid = tag.id
UNION ALL
SELECT comment_hastag_tag.commentid AS messages, tag.name AS tagname, 0 AS num_reply
FROM comment_hastag_tag
JOIN tag ON comment_hastag_tag.tagid = tag.id
UNION ALL
SELECT messages, tagname, num_reply
FROM messagetags
),
checkreplycount AS
(
SELECT messages, tagname, SUM(num_reply)
FROM handle0
GROUP BY messages, tagname
HAVING SUM(num_reply) >= :X
)
SELECT tagname, COUNT(messages)
FROM checkreplycount
GROUP BY tagname
ORDER BY count DESC, tagname
LIMIT 10;

--5--
with req_forum
as 
(
SELECT forum.id  as forum_id , a.name 
FROM forum, person, place a , place b
WHERE forum.ModeratorPersonId = person.id 
AND person.locationcityid = a.id 
AND b.name = :country_name
AND a.PartOfPlaceId = b.id 
),
req_post_tag as
(
SELECT DISTINCT forum_id
FROM req_forum , Post_hasTag_Tag , Post, Tag , TagClass
WHERE req_forum.forum_id = Post.ContainerForumId
AND Post.id = Post_hasTag_Tag.PostId 
AND Post_hasTag_Tag.TagId = Tag.id 
AND Tag.TypeTagClassId = tagclass.id
AND tagclass.name = :tagclass
),
all_tag AS
(
SELECT forum_id , Post_hasTag_Tag.TagId, COUNT(Post_hasTag_Tag.TagId) as tag_count
FROM req_post_tag, Post, Post_hasTag_Tag
WHERE Post.ContainerForumId = req_post_tag.forum_id 
AND Post_hasTag_Tag.PostId = Post.id
GROUP BY forum_id , Post_hasTag_Tag.TagId 
ORDER BY forum_id
),
max_tag AS
(
SELECT forum_id , TagId, tag_count
FROM all_tag
WHERE (forum_id, tag_count) = ANY
(
SELECT forum_id , MAX(tag_count)
FROM all_tag
GROUP BY forum_id 
) 
ORDER BY forum_id
),
max_tag_postmsg AS
(
SELECT forum_id as forumid, title as forumtitle, tag.name as mostpopulartag, COUNT(Post_hasTag_Tag.PostId) as count
FROM max_tag, Forum, Post_hasTag_Tag, Post, Tag
WHERE max_tag.forum_id = Forum.id 
AND Post.ContainerForumId = max_tag.forum_id 
AND Post_hasTag_Tag.PostId = Post.id 
AND Post_hasTag_Tag.TagId =  max_tag.TagId
AND Tag.id = max_tag.TagId
GROUP BY forumid , forumtitle, mostpopulartag
ORDER BY count DESC, forumid, forumtitle,mostpopulartag
)
SELECT * FROM max_tag_postmsg

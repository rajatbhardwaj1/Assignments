--P1--
CREATE INDEX person_knows_person_person1id ON person_knows_person(person1id);
CREATE INDEX person_hasinterest_tag_personid ON person_hasinterest_tag(personid);
CREATE INDEX person_hasinterest_tag_tagid ON person_hasinterest_tag(tagid);
CREATE INDEX tag_id ON tag(id);
CREATE INDEX person_likes_post_postid ON person_likes_post(postid);
CREATE INDEX index_post_id ON post(id);
CREATE INDEX person_likes_comment_commentid ON person_likes_comment(commentid);
CREATE INDEX index_comment_id ON comment(id);

--Q1--
WITH friendsreversed AS
(
    SELECT person1id AS person1sid, person2id AS person2sid
    FROM person_knows_person
    UNION ALL
    SELECT person2id AS person1sid, person1id AS person2sid
    FROM person_knows_person
),
persontagfriends AS
(
    SELECT person_hasinterest_tag.personid, person_hasinterest_tag.tagid, friendsreversed.person2sid AS friend, post.id AS messageid
    FROM person_hasinterest_tag
    JOIN tag ON person_hasinterest_tag.tagid = tag.id
    AND :taglist ::text LIKE '%' || tag.name || '%' ::text
    JOIN friendsreversed ON friendsreversed.person1sid = person_hasinterest_tag.personid
    JOIN post ON post.creatorpersonid = friendsreversed.person2sid AND post.creationdate < :lastdate
    JOIN person_likes_post AS t1 ON t1.personid = person_hasinterest_tag.personid AND post.id = t1.postid

    UNION ALL

    SELECT person_hasinterest_tag.personid, person_hasinterest_tag.tagid, friendsreversed.person2sid AS friend, comment.id AS messageid
    FROM person_hasinterest_tag
    JOIN tag ON person_hasinterest_tag.tagid = tag.id
    AND :taglist ::text LIKE '%' || tag.name || '%' ::text
    JOIN friendsreversed ON friendsreversed.person1sid = person_hasinterest_tag.personid
    JOIN comment ON comment.creatorpersonid = friendsreversed.person2sid AND comment.length > :commentlength
    JOIN person_likes_comment AS t1 ON t1.personid = person_hasinterest_tag.personid AND comment.id = t1.commentid
),
personfriendpairs AS
(
    SELECT t1.personid AS person1sid, t2.personid AS person2sid
    FROM persontagfriends AS t1
    JOIN persontagfriends AS t2 
    ON t1.friend = t2.friend AND t1.tagid = t2.tagid AND t1.messageid = t2.messageid AND t1.personid < t2.personid
    GROUP BY t1.personid, t2.personid
    HAVING COUNT(DISTINCT t1.tagid) >= :K AND COUNT(DISTINCT t1.messageid) >= :X
    EXCEPT 
    SELECT person1id, person2id
    FROM person_knows_person
)
    
SELECT t1.person1sid, t1.person2sid, COUNT(f1.person2sid) AS mutualfriendcount
FROM personfriendpairs AS t1
JOIN friendsreversed AS f1 ON t1.person1sid = f1.person1sid
JOIN friendsreversed AS f2 ON t1.person2sid = f2.person1sid AND f1.person2sid = f2.person2sid
GROUP BY t1.person1sid, t1.person2sid
ORDER BY person1sid, mutualfriendcount DESC, person2sid;

--C1--
DROP INDEX person_knows_person_person1id;
DROP INDEX person_hasinterest_tag_personid;
DROP INDEX person_hasinterest_tag_tagid;
DROP INDEX tag_id;
DROP INDEX person_likes_post_postid;
DROP INDEx index_post_id;
DROP INDEX person_likes_comment_commentid;
DROP INDEX index_comment_id;

--P2--
CREATE INDEX place_partofplaceid ON place(partofplaceid);
CREATE INDEX person_studyat_university_personid ON person_studyat_university(personid);
CREATE INDEX person_knows_person_person1id ON person_knows_person(person1id);

--Q2--
WITH country AS
(
SELECT id AS country_id FROM place WHERE name= :country_name
LIMIT 1
),
accepted_cities AS
(
SELECT id AS cityid 
FROM place 
JOIN country ON country.country_id = place.partofplaceid  
),
shortlisted_person AS
(
SELECT person.id AS personid, person_studyat_university.universityid AS universityid, EXTRACT(MONTH FROM person.birthday) AS birth_month
FROM person
JOIN person_studyat_university ON person_studyat_university.personid = person.id
JOIN accepted_cities ON person.locationcityid = accepted_cities.cityid
WHERE person.creationdate < :enddate and person.creationdate > :startdate
),
personpairs AS
(
SELECT DISTINCT t1.personid AS person1id, t2.personid AS person2id
FROM shortlisted_Person AS t1
JOIN shortlisted_Person AS t2 
ON t1.universityid = t2.universityid AND t1.birth_month = t2.birth_month AND t1.personid < t2.personid
),
p1p2friendcheck AS
(
SELECT personpairs.person1id, personpairs.person2id
FROM personpairs
JOIN person_knows_person
ON personpairs.person1id = person_knows_person.person1id AND personpairs.person2id = person_knows_person.person2id),
p2p3table AS
(
SELECT t1.person1id AS person1, t1.person2id AS person2, t2.person2id AS person3
FROM p1p2friendcheck AS t1
JOIN p1p2friendcheck AS t2 
ON t1.person2id = t2.person1id and t1.person1id < t2.person2id
),
tripletable AS
(
SELECT p2p3table.person1, p2p3table.person2, p2p3table.person3
FROM p2p3table 
JOIN person_knows_person
ON p2p3table.person1 = person_knows_person.person1id AND p2p3table.person3 = person2id 
)

SELECT COUNT(person1) FROM tripletable;

--C2--
DROP INDEX person_knows_person_person1id;
DROP INDEX place_partofplaceid;
DROP INDEX person_studyat_university_personid;

--P3--
CREATE INDEX Comment_hasTag_Tag_creationdate on Comment_hasTag_Tag(creationdate);
CREATE INDEX Post_hasTag_Tag_creationdate on Post_hasTag_Tag(creationdate);
CREATE INDEX Tag_id on tag(id);
CREATE INDEX Tagclass_id on tagclass(id);

--Q3--
WITH Comment_create_mid AS 
(
SELECT tagid, COUNT(tagid) AS ct
FROM Comment_hasTag_Tag
WHERE creationDate >= :begindate
AND creationDate <= :middate
GROUP BY tagid
ORDER BY tagid
),
Comment_mid_end AS 
(
SELECT tagid, COUNT(tagid) AS ct
FROM Comment_hasTag_Tag
WHERE creationDate >= :middate
AND creationDate <= :enddate
GROUP BY tagid
),
post_create_mid AS 
(
SELECT tagid, COUNT(tagid) AS ct
FROM Post_hasTag_Tag
WHERE creationDate >= :begindate
AND creationDate <= :middate
GROUP BY tagid
),
post_mid_end AS 
(
SELECT tagid, COUNT(tagid) AS ct
FROM Post_hasTag_Tag
WHERE creationDate >= :middate
AND creationDate <= :enddate
GROUP BY tagid
),
_msg_create_mid as (
    select *
    from Comment_create_mid
union all
    SELECT *
    FROM post_create_mid
),
_msg_mid_end as 
(
    select *
    from Comment_mid_end
union all
    SELECT *
    FROM post_mid_end
),
msg_create_mid as 
(
select tagid, SUM(ct) as ct
from _msg_create_mid
GROUP BY tagid
),
msg_mid_end as 
(
select tagid, SUM(ct) as ct
from _msg_mid_end
GROUP BY tagid
),
req_tags AS 
(
SELECT t1.tagid
FROM msg_create_mid t1, msg_mid_end t2
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

--C3--
DROP INDEX IF EXISTS Comment_hasTag_Tag_creationdate;
DROP INDEX IF EXISTS Post_hasTag_Tag_creationdate;
DROP INDEX IF EXISTS tag_id;
DROP INDEX IF EXISTS tagclass_id;

--P4--
CREATE INDEX post_id on post(id);
CREATE INDEX comment_parentpostid on comment(parentpostid);
CREATE INDEX comment_id on comment(id);
CREATE INDEX Post_hasTag_Tag_postid on Post_hasTag_Tag(postid);
CREATE INDEX comment_hasTag_Tag_commentid on comment_hasTag_Tag(commentid);

--Q4--
with post_x AS
(
    select post.id  as id 
    FROM post 
    JOIN comment 
    ON parentpostid = post.id 
    GROUP BY post.id
    HAVING count(post.id) >= :X
union
    select post.id as id 
    from post 
    where (:X = 0) OR (:X >0 AND post.id = -1)  
),
post_x_tag as
(
SELECT tagid , count(id) 
FROM post_x , Post_hasTag_Tag
where post_x.id = Post_hasTag_Tag.postid
GROUP BY tagid 
),
comment_x as
( 
    select a.id
    from comment a
    JOIN comment b
    ON a.id = b.parentcommentid
    GROUP BY a.id
    HAVING count(a.id) >= :X 
UNION
SELECT id 
from comment 
where (:X = 0) OR (:X > 0 and id = -1)
),
comment_x_tag as
(
SELECT tagid , count(id) 
FROM comment_x , Comment_hasTag_Tag
where comment_x.id = Comment_hasTag_Tag.commentid
GROUP BY tagid 
),
taglist as
(
SELECT * FROM comment_x_tag union all select * from post_x_tag
),
taglisttot as
(
select tagid , sum(count)as count 
from taglist
GROUP BY tagid 
order by count desc 
limit 10 
),
taglist_final as
(
select name , count 
from taglisttot, tag
where tag.id = taglisttot.tagid
order by count desc , name 
)
SELECT  * FROM taglist_final ;


--C4--
DROP INDEX post_id ;
DROP INDEX comment_id ;
DROP INDEX comment_parentpostid ;
DROP INDEX Post_hasTag_Tag_postid ;
DROP INDEX comment_hasTag_Tag_commentid ;

--P5--
CREATE INDEX person_locationcityid on person(locationcityid);
CREATE INDEX person_id on person(id);
CREATE INDEX place_id on place(id);
CREATE INDEX place_name on place(name);
CREATE INDEX place_partofplace on place(PartOfPlaceId);
CREATE INDEX post_id on post(id);
CREATE INDEX Post_hasTag_Tag_tagid on Post_hasTag_Tag(tagid);
CREATE INDEX Post_hasTag_Tag_postid on Post_hasTag_Tag(PostId);
CREATE INDEX tag_id on tag(id);
CREATE INDEX tagclass_id on tagclass(id);
CREATE INDEX tagclass_name on tagclass(name);
CREATE INDEX post_ContainerForumId on post(ContainerForumId);
CREATE INDEX forum_id on forum(id);
CREATE INDEX forum_moderatorpersonid on forum(ModeratorPersonId);

--Q5--
with req_forum as 
(
SELECT forum.id  as forum_id , a.name 
FROM forum
JOIN person
ON forum.ModeratorPersonId = person.id 
JOIN place a 
ON person.locationcityid = a.id 
JOIN place b
ON b.name = :country_name
AND a.PartOfPlaceId = b.id 
ORDER BY forum_id
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
ORDER BY forum_id
),
all_tag as
(
SELECT forum_id , Post_hasTag_Tag.TagId, COUNT(Post_hasTag_Tag.TagId) as tag_count
FROM req_post_tag, Post, Post_hasTag_Tag
WHERE Post.ContainerForumId = req_post_tag.forum_id 
AND Post_hasTag_Tag.PostId = Post.id
GROUP BY forum_id , Post_hasTag_Tag.TagId 
ORDER BY forum_id
),
max_tag as
(
SELECT forum_id , TagId, tag_count
FROM all_tag
WHERE (forum_id, tag_count) = ANY
    (
    SELECT forum_id , MAX(tag_count) FROM all_tag
    GROUP BY forum_id
    ) 
ORDER BY forum_id
)
SELECT forum_id as forumid, title as forumtitle, tag.name as mostpopulartag, COUNT(Post_hasTag_Tag.PostId) as count
FROM max_tag, Forum, Post_hasTag_Tag, Post, Tag
WHERE max_tag.forum_id = Forum.id 
AND Post.ContainerForumId = max_tag.forum_id 
AND Post_hasTag_Tag.PostId = Post.id 
AND Post_hasTag_Tag.TagId =  max_tag.TagId
AND Tag.id = max_tag.TagId
GROUP BY forumid , forumtitle, mostpopulartag
ORDER BY count DESC, forumid, forumtitle,mostpopulartag;

--C5--
DROP INDEX IF EXISTS person_locationcityid ; 
DROP INDEX IF EXISTS person_id ; 
DROP INDEX IF EXISTS place_id ; 
DROP INDEX IF EXISTS place_name ; 
DROP INDEX IF EXISTS place_partofplace ; 
DROP INDEX IF EXISTS post_id ; 
DROP INDEX IF EXISTS Post_hasTag_Tag_tagid ; 
DROP INDEX IF EXISTS Post_hasTag_Tag_postid ; 
DROP INDEX IF EXISTS tag_id ; 
DROP INDEX IF EXISTS tagclass_id;
DROP INDEX IF EXISTS tagclass_name;
DROP INDEX IF EXISTS post_ContainerForumId;
DROP INDEX IF EXISTS forum_id;
DROP INDEX IF EXISTS forum_moderatorpersonid;
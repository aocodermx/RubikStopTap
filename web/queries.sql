-- Get virtual world rank SINGLE and AVERAGE
select worldRank
  from RanksSingle
  where eventId='333'
    and best > 4064
  order by worldRank
  limit 1;

select worldRank
  from RanksAverage
  where eventId='333'
    and best > 4064
  order by worldRank
  limit 1;


-- Get virtual continental rank SINGLE and AVERAGE
select continentRank
  from RanksSingle
  inner join Persons
  on Persons.id = RanksSingle.PersonId
  where Persons.countryId in
    ( select Id from Countries where Countries.continentId = '_Europe' )
  and RanksSingle.eventId = '333'
  and RanksSingle.best > 4060
  order by RanksSingle.continentRank
  limit 1;

select continentRank
  from RanksAverage
  inner join Persons
  on Persons.id = RanksAverage.PersonId
  where Persons.countryId in
    ( select Id from Countries where Countries.continentId = '_Europe' )
  and RanksAverage.eventId = '333'
  and RanksAverage.best > 4060
  order by RanksAverage.continentRank
  limit 1;


-- Get virtual country rank SINGLE and AVERAGE
select countryRank
  from RanksSingle
  inner join Persons
  on Persons.id = RanksSingle.PersonId
  where countryId = 'Mexico'
    and eventId = '333'
    and best > 4060
  order by countryRank
  limit 1;

select countryRank
  from RanksAverage
  inner join Persons
  on Persons.id = RanksAverage.PersonId
  where countryId = 'Mexico'
    and eventId = '333'
    and best > 4060
  order by countryRank
  limit 1;

--
-- NOTES
--
-- show index from RanksSingle;
-- analyze table Persons, RanksSingle, RanksAverage;
-- alter table Persons add index persons_index ( id, countryId );
-- alter table Countries add index countries_index ( id, name, continentId );
-- alter table RanksSingle add index continent_index_2 ( personId, eventId, best, continentRank );
-- drop index continent_index_2 on RanksSingle;

--
-- Index for improve order by clausule
-- create index continent_index on RanksSingle ( continentRank(11) );
-- alter table RanksSingle add index continent_index ( continentRank );


-- rubikstoptap.php?action=worldsingle&cube=333&time=4064
-- Get virtual world rank SINGLE
select worldRank from RanksSingle where eventId='333' and best=4064 limit 1;

-- Get virtual world rank AGERAGE
select worldRank from RanksAverage where eventId='333' and best=4064 limit 1;

-- Get virtual continental rank SINGLE
-- Get virtual continental rank AVERAGE

-- Get virtual country rank SINGLE
-- Get virtual country rank AVERAGE

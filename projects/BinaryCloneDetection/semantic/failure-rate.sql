-- Computes statistics about false negatives in a database populated with two or more specimens compiled in different ways.
begin transaction;

drop table if exists fr_results;
drop table if exists fr_false_positives;
drop table if exists fr_false_negatives;
drop table if exists fr_clone_pairs;
drop table if exists fr_negative_pairs;
drop table if exists fr_positive_pairs;
drop table if exists fr_function_pairs;
drop index if exists fr_fio_func_id;
drop table if exists fr_fio;
drop table if exists fr_functions;
drop table if exists fr_funcnames;
drop table if exists fr_specimens;
drop table if exists fr_settings;

create table fr_settings as
    select 0.75 as similarity_threshold;

-- Files that are binary specimens; i.e., not shared libraries, etc.
create table fr_specimens as
    select distinct specimen_id as id
    from semantic_functions;

-- Function names that are present exactly once in each specimen
-- And which contain a certain number of instructions                                           !!FIXME: should be a parameter
-- And for which no test tried to consume too many inputs (status<>911000007)
-- And which come from the specimen, not from a dynamic library
create table fr_funcnames as
    select func1.name as name
        from fr_specimens as file
        join semantic_functions as func1 on func1.file_id = file.id
        left join semantic_functions as func2
            on func1.id<>func2.id and func1.name <> '' and func1.name=func2.name and func1.file_id=func2.file_id
        where func2.id is null and func1.ninsns >= 2                                            -- !!FIXME
        group by func1.name
        having count(*) = (select count(*) from fr_specimens)
--     except (
--       select func.name as name
--           from semantic_fio as fio
--           join semantic_functions as func on fio.func_id = func.id
--           where fio.globals_consumed > 0
--     )
;


-- Functions that have a name and which appear once per specimen by that name.
create table fr_functions as
    select distinct func.*
        from fr_funcnames as named
        join semantic_functions as func on named.name = func.name

        -- uncomment the following lines to consider only those functions that passed at least once
--      join semantic_fio as test on func.id=test.func_id and test.status=0
;

-- Tests that we want to consider
create table fr_fio as
    select fio.*
        from fr_functions as func
        join semantic_fio as fio on func.id = fio.func_id;
create index fr_fio_func_id on fr_fio(func_id);

-- Select all pairs of functions from two different specimens
create table fr_function_pairs as
    select distinct f1.id as func1_id, f2.id as func2_id
        from fr_functions as f1
        join fr_functions as f2 on f1.id < f2.id and f1.specimen_id <> f2.specimen_id

        -- Uncomment the following lines to consider only those pairs of functions where both functions passed for
        -- at least one of the same input groups
        join fr_fio as test1 on f1.id = test1.func_id
        join fr_fio as test2 on f2.id = test2.func_id and test1.igroup_id = test2.igroup_id
        where test1.status = 0 and test2.status = 0

        -- Uncomment the following lines to consider only those pairs of functions where all tests were successful
--      except
--          select  f1.id as func1_id, f2.id as func2_id
--          from fr_functions as f1
--          join fr_functions as f2 on f1.name = f2.name and f1.id < f2.id and f1.specimen_id <> f2.specimen_id
--          join fr_fio as test1 on f1.id = test1.func_id
--          join fr_fio as test2 on f2.id = test2.func_id
--          where test1.status <> 0 or test2.status <> 0
;



-- Pairs of functions that should have been detected as being similar.  We're assuming that any pair of binary functions that
-- have the same name are in fact the same function at the source level and should therefore be similar.
create table fr_positive_pairs as
    select pair.*
        from fr_function_pairs as pair
        join fr_functions as f1 on pair.func1_id = f1.id
        join fr_functions as f2 on pair.func2_id = f2.id
        where f1.name = f2.name;

-- Pairs of functions that should not have been detected as being similar.  We're assuming that if the two functions have
-- different names then they are different functions in the source code, and that no two functions in source code are similar.
-- That second assumption almost certainly doesn't hold water!
create table fr_negative_pairs as
    select pair.*
        from fr_function_pairs as pair
        join fr_functions as f1 on pair.func1_id = f1.id
        join fr_functions as f2 on pair.func2_id = f2.id
        where f1.name <> f2.name;

-- Pairs of functions that were _detected_ as being similar.
create table fr_clone_pairs as
    select func1_id, func2_id
        from semantic_funcsim where similarity >= (select similarity_threshold from fr_settings);

-- Table of false negative pairs.  These are pairs of functions that were not determined to be similar but which are present
-- in the fr_positives_pairs table.
create table fr_false_negatives as
    select * from fr_positive_pairs except select func1_id, func2_id from fr_clone_pairs;

-- Table of false positive pairs.  These are pairs of functions that were determined to be similar but which are present
-- in the fr_negative_pairs table.
create table fr_false_positives as
    select * from fr_negative_pairs intersect select func1_id, func2_id from fr_clone_pairs;

-- False negative rate is the ratio of false negatives to expected positives
-- False positive rate is the ratio of false positives to expected negatives
create table fr_results as
    select
        (select count(*) from fr_positive_pairs) as "Expected Positives",
        (select count(*) from fr_negative_pairs) as "Expected Negatives",
        (select count(*) from fr_false_negatives) as "False Negatives",
        (select count(*) from fr_false_positives) as "False Positives",
        ((select 100.0*count(*) from fr_false_negatives) / (select count(*) from fr_positive_pairs)) as "FN Percent",
        ((select 100.0*count(*) from fr_false_positives) / (select count(*) from fr_negative_pairs)) as "FP Percent";

-------------------------------------------------------------------------------------------------------------------------------
-- Some queries to show the results

select * from fr_results;

-- select 'The following table contains all the functions of interest: functions that
-- * appear in all specimens exactly once each
-- * have a certain number of instructions, and
-- * didn''t try to consume too many inputs, and
-- * are defined in a specimen, not a dynamic library' as "Notice";
-- select * from fr_functions order by name, specimen_id;

select 'The following table shows the termination status for all
tests performed on the functions of interest.' as "Notice";
select
        fault.name as status,
        count(*) as ntests,
        100.0*count(*)/(select count(*) from fr_fio) as percent
    from fr_fio as fio
    join semantic_faults as fault on fio.status = fault.id
    group by fault.id, fault.name;

select 'The following table shows the false negative function pairs.
Both functions of the pair always have the same name.' as "Notice";
select
        func1.name as name,
        sim.func1_id, sim.func2_id, sim.similarity, sim.ncompares
    from fr_false_negatives as falseneg
    join fr_functions as func1 on falseneg.func1_id = func1.id
    join fr_functions as func2 on falseneg.func2_id = func2.id
    join semantic_funcsim as sim on falseneg.func1_id=sim.func1_id and falseneg.func2_id=sim.func2_id
    order by func1.name;

select 'The following table shows the status breakdown for tests
that were performed on each function of interest.' as "Notice";

select
        func.id as func_id,
        func.name as func_name,
        func.file_id as file_id,
        fault.name as status,
        count(*) as ntests
    from fr_functions as func
    join fr_fio as fio on fio.func_id = func.id
    join semantic_faults as fault on fault.id = fio.status
    group by func.id, func.name, func.file_id, fault.name
    order by func.name, func.file_id;

select 'The following table shows a list of function names that are
false negatives, and the average number of instructions executed by
tests on this name.' as "Notice";
select
        func.name, func.id, func.file_id,
	count(*) as npass,
        sum(fio.instructions_executed)/count(*) as ave_insns_exec
    from fr_fio as fio
    join fr_functions as func on fio.func_id = func.id
    join fr_false_negatives as neg on fio.func_id=neg.func1_id or fio.func_id=neg.func2_id
    where fio.status = 0
    group by func.name, func.id, func.file_id
    order by ave_insns_exec;


rollback;

#!/usr/bin/env python

from datetime import datetime
from rcdb.provider import RCDBProvider
from rcdb.model import ConditionType

# Create RCDBProvider object that connects to DB and provide most of the functions

##db = RCDBProvider("mysql://rcdb_ro@clondb1/rcdb")
db = RCDBProvider("mysql://rcdb:e1tocome@clondb1/rcdb")
#
#db.add_condition(803, "beam_energy", 6.423, datetime(2017, 2, 6, 15, 28, 12, 111111), replace=True)
#
file=open("rcdb-rgm-target-r15807.txt", "r+")
for line in file:
    values = line.split()
    irun=values[0]
#    event_count=float(values[5])*1000000
#    evio_files_count=int(values[6])
    target=values[1]
##    run=db.create_run(irun)

    db.add_condition(irun, "target",target, replace=True)

    print('Run ', irun, ' ', values[0],' ',values[1], ' ', target)

file.close()

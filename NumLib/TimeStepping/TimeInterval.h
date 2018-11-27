/**
 *
 * \copyright
 * Copyright (c) 2012-2018, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 * File:   TimeInterval.h
 *
 * Created on November 26, 2018, 4:44 PM
 */
#pragma once

namespace NumLib
{
/*!
 * Class for a time interval, which has a member to check whether the given time
 * is in this time interval.
 */
class TimeInterval
{
public:
    TimeInterval(const double start_time, const double end_time)
        : _start_time(start_time), _end_time(end_time)
    {
    }

    bool isInThisTimeInterval(const double current_time) const
    {
        return (current_time >= _start_time && current_time <= _end_time)
                   ? true
                   : false;
    }

private:
    const double _start_time;
    const double _end_time;
};

}  // end of namespace
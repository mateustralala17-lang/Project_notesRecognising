#pragma once
#include <vector>
#include <set>
#include <map>
#include <string>
#include "DataTypes.h"
#include "MusicUtils.h"

class IntervalRecognizer
{
    private:
        std::map<int, std::string> intervalTemplates;

    public:
        IntervalRecognizer();        
        std::set<IntervalData> findAllIntervals(const std::set<int>& pitches);
};
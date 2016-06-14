#include <map>
#include <iostream>
#include <boost/assign/list_of.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include "name_map.h"



using namespace std;


int front_side = 0, right_side = 1, back_side = 2, left_side = 3, top_side = 4;
string small = "s", medium = "m", large ="l";
string robot1 = "boticelli", robot2 = "leonardo", robot3 = "raphael", robot4 = "donatello";
string robot5 =  "titian", robot6 =  "masaccio", robot7 =  "michelangelo";
string robot8 =  "ghiberti", robot9 =  "giotto", robot10 =  "bellini";

map<int, boost::tuple<std::string, int, std::string> > name_map = boost::assign::map_list_of
//   marker\_id, 		  name,   side,	    size
	(2,	boost::make_tuple(robot1, top_side, large))
	(102,	boost::make_tuple(robot1, front_side, medium))
	(202,	boost::make_tuple(robot1, front_side, small))
	(104,	boost::make_tuple(robot1, right_side, medium))
	(105,	boost::make_tuple(robot1, right_side, small))
	(106,	boost::make_tuple(robot1, left_side, medium))
	(107,	boost::make_tuple(robot1, left_side, small))

	(20,	boost::make_tuple(robot2, top_side, large))
	(2202,	boost::make_tuple(robot2, front_side, medium))
	(203,	boost::make_tuple(robot2, front_side, small))
	(204,	boost::make_tuple(robot2, right_side, medium))
	(205,	boost::make_tuple(robot2, right_side, small))
	(206,	boost::make_tuple(robot2, left_side, medium))
	(207,	boost::make_tuple(robot2, left_side, small))

	(4,	boost::make_tuple(robot3, top_side, large))
	(302,	boost::make_tuple(robot3, front_side, medium))
	(303,	boost::make_tuple(robot3, front_side, small))
	(304,	boost::make_tuple(robot3, right_side, medium))
	(305,	boost::make_tuple(robot3, right_side, small))
	(306,	boost::make_tuple(robot3, left_side, medium))
	(307,	boost::make_tuple(robot3, left_side, small))

	(5,	boost::make_tuple(robot4, top_side, large))
	(402,	boost::make_tuple(robot4, front_side, medium))
	(403,	boost::make_tuple(robot4, front_side, small))
	(404,	boost::make_tuple(robot4, right_side, medium))
	(405,	boost::make_tuple(robot4, right_side, small))
	(406,	boost::make_tuple(robot4, left_side, medium))
	(407,	boost::make_tuple(robot4, left_side, small))

	(7,	boost::make_tuple(robot5, top_side, large))
	(502,	boost::make_tuple(robot5, front_side, medium))
	(503,	boost::make_tuple(robot5, front_side, small))
	(504,	boost::make_tuple(robot5, right_side, medium))
	(505,	boost::make_tuple(robot5, right_side, small))
	(506,	boost::make_tuple(robot5, left_side, medium))
	(507,	boost::make_tuple(robot5, left_side, small))

	(3,	boost::make_tuple(robot6, top_side, large))
	(602,	boost::make_tuple(robot6, front_side, medium))
	(603,	boost::make_tuple(robot6, front_side, small))
	(604,	boost::make_tuple(robot6, right_side, medium))
	(605,	boost::make_tuple(robot6, right_side, small))
	(606,	boost::make_tuple(robot6, left_side, medium))
	(607,	boost::make_tuple(robot6, left_side, small))

	(15,	boost::make_tuple(robot7, top_side, large))
	(702,	boost::make_tuple(robot7, front_side, medium))
	(703,	boost::make_tuple(robot7, front_side, small))
	(704,	boost::make_tuple(robot7, right_side, medium))
	(705,	boost::make_tuple(robot7, right_side, small))
	(706,	boost::make_tuple(robot7, left_side, medium))
	(707,	boost::make_tuple(robot7, left_side, small))

	(80,	boost::make_tuple(robot8, top_side, large))
	(802,	boost::make_tuple(robot8, front_side, medium))
	(803,	boost::make_tuple(robot8, front_side, small))
	(804,	boost::make_tuple(robot8, right_side, medium))
	(805,	boost::make_tuple(robot8, right_side, small))
	(806,	boost::make_tuple(robot8, left_side, medium))
	(807,	boost::make_tuple(robot8, left_side, small))

	(10,	boost::make_tuple(robot9, top_side, large))
	(902,	boost::make_tuple(robot9, front_side, medium))
	(903,	boost::make_tuple(robot9, front_side, small))
	(904,	boost::make_tuple(robot9, right_side, medium))
	(905,	boost::make_tuple(robot9, right_side, small))
	(906,	boost::make_tuple(robot9, left_side, medium))
	(907,	boost::make_tuple(robot9, left_side, small))

	(2,	boost::make_tuple(robot10, top_side, large))
	(1002,	boost::make_tuple(robot10, front_side, medium))
	(1003,	boost::make_tuple(robot10, front_side, small))
	(1004,	boost::make_tuple(robot10, right_side, medium))
	(1005,	boost::make_tuple(robot10, right_side, small))
	(1006,	boost::make_tuple(robot10, left_side, medium))
	(1007,	boost::make_tuple(robot10, left_side, small));






/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: SearchRelay.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "SearchRelay.h"

//---------------------------------------------------------
// Constructor

SearchRelay::SearchRelay()
{
	normal_indices_five = std::vector<double> (19, 0);
	normal_indices_one = std::vector<double> (19, 0);
	fudge_factor = 5; //m
}

//---------------------------------------------------------
// Destructor

SearchRelay::~SearchRelay()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SearchRelay::OnNewMail(MOOSMSG_LIST &NewMail)
{
   MOOSMSG_LIST::iterator p;
   
   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      std::string key = msg.GetKey();
      //all
      if(key=="GPS_PTIME"){
    	  now = pt::time_from_string(msg.GetString());
		  if(my_role=="shore"){
			  if(last.is_pos_infinity()){last=pt::time_from_string(msg.GetString());}
		  }
      }
      //relay
      else if(key=="SEARCH_RELAY_RESET"){

            }

      else if(key=="ACOMMS_SNR_OUT"){
    	  //Here assuming last nav variables is current position
    	  //Update the mean and var of one node if current position is within
    	  //a fudge factor of that node

    	  int closest_ind = closest_vertex(myx,myy);
    	  double closest_dist = sqrt(pow((seglist.get_vx(closest_ind)-myx),2) + pow((seglist.get_vy(closest_ind)-myy),2));
    	  std::cout<<closest_dist<<std::endl;
    	  if(closest_dist<fudge_factor){
    		data[closest_ind].push_back(msg.GetDouble());
    		mean[closest_ind] = gsl_stats_mean(&(data[closest_ind][0]),1,data[closest_ind].size());
    	    var[closest_ind] = gsl_stats_variance(&(data[closest_ind][0]),1,data[closest_ind].size());
    	    std::cout<<"Updating Mean and Var for Point "<<closest_ind<<std::endl;
    	    std::cout<<seglist.get_vx(closest_ind)<<" , "<<seglist.get_vy(closest_ind)<< std::endl;
    	    std::cout<<mean[closest_ind]<<" , "<<var[closest_ind]<< std::endl;
    	  }
      }
      //shore
      else if(key=="SEARCH_RELAY_WAIT_TIME"){
    	  wait_time = pt::seconds((long) msg.GetDouble());
      }
      else if(key=="SEARCH_RELAY_RATE"){
    	  rate = (int) msg.GetDouble();
      }
      else if(key=="NAV_X"){
    	  myx = msg.GetDouble();
      }
      else if(key=="NAV_Y"){
    	  myy = msg.GetDouble();
      }

   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SearchRelay::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", is_float(int));
	
	m_MissionReader.GetConfigurationParam("Role",my_role);

	m_Comms.Register("GPS_PTIME",0);

	if(my_role=="relay"){
		m_MissionReader.GetConfigurationParam("Mode",mode);
		m_Comms.Register("ACOMMS_SNR_OUT",0);
		m_Comms.Register("SEARCH_RELAY_RESET",0);
		m_Comms.Register("NAV_X",0);
		m_Comms.Register("NAV_Y",0);

		if(mode=="normal"){
			double temp_normal_indices_five[19] = {10.141,1.1656,0.6193,0.4478,0.359,0.3035,0.2645,
					0.2353,0.2123,0.1109,0.0761,0.0582,0.0472,0.0397,0.0343,0.0302,0.0269,0.0244};
			memcpy( &temp_normal_indices_five[0], &normal_indices_five[0], sizeof(temp_normal_indices_five[0])*19 );
			double temp_normal_indices_one[19] = {39.3343,3.102,1.3428,0.9052,0.7054,0.5901,0.5123,0.4556,0.4119,
									0.223,0.1579,0.1235,0.1019,0.087,0.076,0.0675,0.0608,0.0554};
			memcpy( &temp_normal_indices_one[0], &normal_indices_one[0], sizeof(temp_normal_indices_one[0])*19 );
			}

		GetWaypoints();

		m_Comms.Notify("RELAY_STATUS","ready");
		}

	else if(my_role=="end_node"){
		m_Comms.Notify("END_STATUS","ready");
	}

	else if(my_role=="shore_node"){
		m_Comms.Register("ACOMMS_TRANSMIT_SIMPLE",0);
		m_Comms.Register("ACOMMS_RECEIVED_SIMPLE",0);
		m_Comms.Register("SEARCH_RELAY_WAIT_TIME",0);
		m_Comms.Register("SEARCH_RELAY_RATE",0);
		m_Comms.Register("RELAY_STATUS",0);
		m_Comms.Register("END_STATUS",0);
		wait_time = pt::seconds(20);
		rate = 2;
		last = pt::ptime(pt::pos_infin);
	}

   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool SearchRelay::Iterate()
{
   // happens AppTick times per second

	if(my_role=="relay"){

	}

	else if(my_role=="end_node"){ //do nothing

	}

	else if(my_role=="shore_node" && relay_status=="ready" && end_status=="ready"){ //transmitting every wait_time seconds

		if(now-last>=wait_time){
			int length;
			std::stringstream ss;
			ss << counter;
			if(rate==0){length = 32;}
			else if(rate==2){length = 192;}
			std::string mail = ss.str()+"---"+getRandomString(length);
			m_Comms.Notify("ACOMMS_TRANSMIT_DATA",mail);
		}
	}

   return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SearchRelay::OnStartUp()
{
   // happens before connection is open
	
   return(true);
}

void SearchRelay::ComputeIndices(){

}

void SearchRelay::GetWaypoints(){
	std::string filename = "relay_waypoints.txt";
	std::string one_point;
	std::ifstream waypointsfile("relay_waypoints.txt",std::ifstream::in);

	while(waypointsfile.good()){
				getline(waypointsfile,one_point);
				int pos = one_point.find(',');
	if(pos>=0){
				std::string subx = one_point.substr(0,pos-1);
				std::string suby = one_point.substr(pos+1);
				seglist.add_vertex(atof(subx.c_str()),atof(suby.c_str()));
			}
	}
}

std::string SearchRelay::getRandomString( int length ) {
    srand((unsigned) time(NULL));

    std::stringstream ss;
    const int passLen = length;
    for (int i = 0; i < passLen; i++) {
    	char num = (char) ( rand() % 62 );
    	if ( num < 10 )
    		num += '0';
    	else if ( num < 36 )
    		num += 'A'-10;
    	else
    		num += 'a'-36;
    	ss << num;
    }

    return ss.str();
}

unsigned int SearchRelay::closest_vertex(double x, double y)
{
	unsigned int vsize = seglist.size();

	if(vsize == 0)
		return(0);

	  double closest_dist = sqrt(pow((seglist.get_vx(0)-myx),2) + pow((seglist.get_vy(0)-myy),2));

  unsigned int i, ix = 0;
  for(i=1; i<vsize; i++) {
	  double idist = sqrt(pow((seglist.get_vx(i)-myx),2) + pow((seglist.get_vy(i)-myy),2));
    if(idist < closest_dist) {
      closest_dist = idist;
      ix = i;
    }
  }
  return(ix);
}

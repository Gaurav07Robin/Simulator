/*
FOR THE WORLD TO BE DYNAMICALLY GROWING ->
	We need the logica; matrix like structure but list like additions too...  if that's possible

	ACCOMPLISHED -> That is what i wrote the Graph_Mat for !!
*/

#pragma once

#include "log.hpp"
#include "Entities/snake.hpp"
#include "world_plot.hpp"

#include <vector>
#include <utility>
#include <unordered_map>
#include <thread>
#include <mutex>

typedef World* World_Ptr;
class World: public _ID{
	typedef int32_t dimen_t;
	typedef coord coord_type;

	typedef std::make_unsigned_t<int32_t> udimen_t;
public:
	WorldPlot world_plot;

	_timePoint getCurrentTime() const noexcept;

	// @note - the below two functions are complementary to the other
	// @note2 - ateFood() is expected NEVER to fail, based on the food_mutex, hence returns void
	std::mutex food_mutex;	// locks JUST AFTER Snake::eatFood() starts and BEFORE World::ateFood(Snake*)
	const coord& getFoodCoord();
	bool lockFood(const Snake*);
	void ateFood(const Snake*); //which snake ate it, log it, then create the food again

	//------constants for this world------//
	int _MAX_NumSnakes = 4;
	int _init_SnakeLength = 2;
	//------                ------//

	// --Just abstracted access to private worldPlot member function, no logic in these of their own-- //
	void getShortestPathToFood( const Entity_Point& origin, directionalPath& ) const;
	// x-Just abstracted access to private worldPlot member function, no logic in these of their own-x //

	struct {
		// @CAUTION - Ensure this access is thread safe
		// this will only be SET IN CONSTRUCTOR AND STOP_SIMULATION, for GET, use the method
			std::atomic_bool _world_runnning{true}; //world will keep moving forward (ie. entities will keep existing and acting)
		public:
			auto is_world_running() const{ return _world_runnning.load(); }
			auto reset_world_running(){
				// @caution - Lock the mutex here
				this->_world_runnning = false;
			}
			//auto get_world_thread_id() const{ return std::this_thread::get_id(); }
	} _shared_concurrent_data;

	//wil be required to join these threads, in stopSimulation();
	std::vector< std::thread > entity_threads;  // not a concurrently access data, since ONLY to be used by stopSimulation and startSimulation()
	std::vector< Entity* > entities;	// accessed by World_Plot::createFood()

	udimen_t getWorldDimen() const;

	Graph_Box_3D<Box>* get_box(const coord& pos) {
		return this->world_plot.get_box(pos);
	}

	const Graph_Box_3D<Box>* get_box(const coord& pos) const{
		return this->world_plot.get_box(pos);
	}

	World( const World_Ptr, _timePoint );  //can later be made private

private:
	// std::vector<Log> logs;   // these logs may no longer be required and managed by world_node
	// State currentState;	// @deprecated - No more will the world be the one to store state, now it will be managed by World_Node
	bool simulationRunning;

	std::mutex __world_mutex;

	// std::pair<uint16_t, uint16_t> _WorldDimens; //current dimensions of this world
	// std::pair<uint16_t, uint16_t> _curr_BOUNDS; //current `reserved` dimensions of this world

	//udimen_t _curr_BOUND; //current `reserved` `order` of this world

	Path_Finder path_finder;

	// bool _CheckBounds();    //for checking `need` to increase size

	bool _RangeCheck(const coord_type&) const;    //for checking if a coordinate is valid

	State capture_state();

	void runNextUnitTime();   //resumes the world, the nextTime period happens in this time
		// @todo - This is the constructor that creates the new world just after big bang
	World();
	~World();

	// friend class Verse;  // doesn't need to be a friend, since World_Node is the one that needs that private constructor
	friend class World_Node;

};

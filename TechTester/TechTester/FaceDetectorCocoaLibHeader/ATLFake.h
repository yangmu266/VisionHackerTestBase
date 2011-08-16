//
//  ATLFake.h
//  MultiFace_Mac
//
//  Created by Tang Wenbin on 4/23/11.
//  Copyright 2011 Tsinghua University. All rights reserved.
//
#ifndef __ATL_FAKE__
#define __ATL_FAKE__


#include <vector>
#include <iostream>
using std::vector;

namespace ATL
{
	
	typedef int POSITION;

	
	template< typename E >
	class CAtlList
	{
	public:
		vector<E>   list;
		
	public:
		CAtlList()
		{
			list.clear();
		}
		
		~CAtlList()
		{
			list.clear();
		}
		
		int GetCount()
		{
			
			return list.size();
		}
		
		bool IsEmpty()
		{
			return list.size() == 0;
		}
		
		E& GetHead()
		{
			if (list.size() == 0) {
				std::cerr<<"GetHead Error: out of range!"<<std::endl;
				throw "ATLFake: out of range!";
			}
			return list[0];
		}
		
		const E& GetHead() const
		{
			if (list.size() == 0) {
				std::cerr<<"GetHead Error: out of range!"<<std::endl;
				throw "ATLFake: out of range!";
			}
			return list[0];
		}
		
		POSITION GetHeadPosition()
		{
			if (list.size() == 0) return -1;
			return 0;
		}
		
		E& GetNext(POSITION& pos)
		{
			if (pos < 0 || pos > list.size()-1) {
				std::cerr<<"GetNext Error: out of range! Pos: "<<pos<<" Size: "<<list.size()<<std::endl;
				throw "ATLFake: out of range!";
			}
			if (pos == list.size()-1) {
				pos = -1;
				return list[list.size()-1];
			}
			return list[pos ++];
		}
		
		const E& GetNext(POSITION& pos) const
		{
			if (pos < 0 || pos > list.size()-1) {
				std::cerr<<"GetNext Error: out of range! Pos: "<<pos<<" Size: "<<list.size()<<std::endl;
				throw "ATLFake: out of range!";
			}
			if (pos == list.size()-1) {
				pos = -1;
				return list[list.size()-1];
			}
			return list[pos ++];
		}
		
		void RemoveAll()
		{
			list.clear();
		}
		
		bool SetCount( size_t nNewSize, int nGrowBy = -1 )
		{
			list.resize(nNewSize);
			return true;
		}
		
		int Add()
		{
			list.resize( list.size() + 1 );
			return list.size();
		}
		
		int Add(const E& elem)
		{
			list.push_back( elem );
			return list.size();
		}
		
		POSITION AddTail()
		{
			Add();
			return list.size() - 1;
		}
		
		POSITION AddTail(const E& elem)
		{
			Add(elem);
			return list.size() - 1;
		}
		
		void RemoveAt( POSITION pos )
		{
			if (pos < 0 || pos > list.size()-1) {
				std::cerr<<"RemoveAt Error: out of range! Pos: "<<pos<<" Size: "<<list.size()<<std::endl;
				throw "ATLFake: out of range!";
			}
			list.erase(list.begin() + pos);
		}
		
		E& operator [] (int pos)
		{
			if (pos < 0 || pos > list.size()-1) {
				std::cerr<<"[] Error: out of range! Pos: "<<pos<<" Size: "<<list.size()<<std::endl;
				throw "ATLFake: out of range!";
			}
			return list[pos];
		}
		
		const E& operator [] (int pos) const
		{
			if (pos < 0 || pos > list.size()-1) {
				std::cerr<<"[] Error: out of range! Pos: "<<pos<<" Size: "<<list.size()<<std::endl;
				throw "ATLFake: out of range!";
			}
			return list[pos];
		}
		
	};
	
}

#define CAtlArray CAtlList

#endif //__ATL_FAKE__
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * All rights reserved.                                             *
// ********************************************************************
//
/// @library DataLib
//
/// @file NuclearData.h
//
/// @class DataLib::NuclearData
//
/// @brief Header of std-compatible class for the Nuclear Data. Without direct object restore.
//
/// @author Kirill Chernov, Chernov_KG@nrcki.ru
//
/// @version 0.9.6
//

#pragma once

#include <iostream>

namespace DataLib
{
	class NuclearData
	{
	public:

		NuclearData()
			: _iConfig(0),
			_iTM(0),
			_iResonance(0),
			_iUrr(0),
			_nTM_temperatures(0),
			_TM_tleft(0.),
			_TM_tright(0.),
			_n_temperatures(0),
			_tleft(0.),
			_tright(0.),
			_isotropic(0),
			_fission_type(0),
			_iName(0),
			changedStatus(false)
		{}

		// For back compatibility only!
		NuclearData(
			/* for information only */
			/* 20 bits */ //int iConfig = 0,
			/* 2 bits */ int iTM,
			/* 2 bits */ int iResonance,
			/* 1 bit  */ int iUrr,
			/* 2 bits */ int nTM_temperatures,
			/* 8 bytes */ double TM_tleft,
			/* 8 bytes */ double TM_tright,
			/* 2 bits */ int n_temperatures,
			/* 8 bytes */ double tleft,
			/* 8 bytes */ double tright,
			/* 1 bit  */ int isotropic,
			/* 2 bit  */ //int fission_type = 0,
			/* 31 bits */ int iName // Only positive values
		) : _iConfig(0),
			_iTM(iTM),
			_iResonance(iResonance),
			_iUrr(iUrr),
			_nTM_temperatures(nTM_temperatures),
			_TM_tleft(TM_tleft),
			_TM_tright(TM_tright),
			_n_temperatures(n_temperatures),
			_tleft(tleft),
			_tright(tright),
			_isotropic(isotropic),
			_fission_type(0),
			_iName(iName),
			changedStatus(false)
		{}

		// For back compatibility only!
		NuclearData(
			/* for information only */
			/* 20 bits */ //int iConfig = 0,
			/* 2 bits */ int iTM,
			/* 2 bits */ int iResonance,
			/* 1 bit  */ int iUrr,
			/* 2 bits */ int nTM_temperatures,
			/* 8 bytes */ double TM_tleft,
			/* 8 bytes */ double TM_tright,
			/* 2 bits */ int n_temperatures,
			/* 8 bytes */ double tleft,
			/* 8 bytes */ double tright,
			/* 1 bit  */ int isotropic,
			/* 2 bit  */ int fission_type,
			/* 31 bits */ int iName // Only positive values
		) : _iConfig(0),
			_iTM(iTM),
			_iResonance(iResonance),
			_iUrr(iUrr),
			_nTM_temperatures(nTM_temperatures),
			_TM_tleft(TM_tleft),
			_TM_tright(TM_tright),
			_n_temperatures(n_temperatures),
			_tleft(tleft),
			_tright(tright),
			_isotropic(isotropic),
			_fission_type(0),
			_iName(iName),
			changedStatus(false)
		{}

		NuclearData(
			/* for information only */
			/* 20 bits */ int iConfig,
			/* 2 bits */ int iTM,
			/* 2 bits */ int iResonance,
			/* 1 bit  */ int iUrr,
			/* 2 bits */ int nTM_temperatures,
			/* 8 bytes */ double TM_tleft,
			/* 8 bytes */ double TM_tright,
			/* 2 bits */ int n_temperatures,
			/* 8 bytes */ double tleft,
			/* 8 bytes */ double tright,
			/* 1 bit  */ int isotropic,
			/* 2 bit  */ int fission_type,
			/* 31 bits */ int iName // Only positive values
		) : _iConfig(iConfig),
			_iTM(iTM),
			_iResonance(iResonance),
			_iUrr(iUrr),
			_nTM_temperatures(nTM_temperatures),
			_TM_tleft(TM_tleft),
			_TM_tright(TM_tright),
			_n_temperatures(n_temperatures),
			_tleft(tleft),
			_tright(tright),
			_isotropic(isotropic),
			_fission_type(fission_type),
			_iName(iName),
			changedStatus(false)
		{}

		// Adapter for external classes. Class T has to implement all getters!
		template<typename T>
		NuclearData(const T& external)
			:NuclearData(
				external.iConfig(),
				external.iTM(),
				external.iResonance(),
				external.iUrr(),
				external.nTM_temperatures(),
				external.TM_tleft(),
				external.TM_tright(),
				external.n_temperatures(),
				external.tleft(),
				external.tright(),
				external.isotropic(),
				external.fission_type(),
				external.iName()
			) {}

		NuclearData(const NuclearData& other)
			: _iConfig{ other._iConfig },
			_iTM{ other._iTM },
			_iResonance{ other._iResonance },
			_iUrr{ other._iUrr },
			_nTM_temperatures{ other._nTM_temperatures },
			_TM_tleft{ other._TM_tleft },
			_TM_tright{ other._TM_tright },
			_n_temperatures{ other._n_temperatures },
			_tleft{ other._tleft },
			_tright{ other._tright },
			_isotropic{ other._isotropic },
			_fission_type{ other._fission_type },
			_iName{ other._iName },
			changedStatus(false)
		{}

		// Adapter for external classes. Class T has to implement all getters!
		template<typename T>
		void place(const T& external)
		{
			_iConfig = external.iConfig;
			_iTM = external.iTM;
			_iResonance = external.iResonance;
			_iUrr = external.iUrr;
			_nTM_temperatures = external.nTM_temperatures;
			_TM_tleft = external.TM_tleft;
			_TM_tright = external.TM_tright;
			_n_temperatures = external.n_temperatures;
			_tleft = external.tleft;
			_tright = external.tright;
			_isotropic = external.isotropic;
			_fission_type = external.fission_type;
			_iName = external.iName;
			changedStatus = false;
		}

		int iConfig() const
		{
			return _iConfig;
		}

		void iConfig(int config)
		{
			changedStatus = false;
			_iConfig = config;
		}

		int iTM() const
		{
			return _iTM;
		}

		void iTM(int m_tm)
		{
			changedStatus = false;
			_iTM = m_tm;
		}

		int iResonance() const
		{
			return _iResonance;
		}

		void iResonance(int m_resonance)
		{
			changedStatus = false;
			_iResonance = m_resonance;
		}

		int iUrr() const
		{
			return _iUrr;
		}

		void iUrr(int m_unresolved)
		{
			changedStatus = false;
			_iUrr = m_unresolved;
		}

		int nTM_temperatures() const
		{
			return _nTM_temperatures;
		}

		void nTM_temperatures(int n_tm_temperatures)
		{
			changedStatus = false;
			_nTM_temperatures = n_tm_temperatures;
		}

		double TM_tleft() const
		{
			return _TM_tleft;
		}

		void TM_tleft(double tm_temperature_left)
		{
			changedStatus = false;
			_TM_tleft = tm_temperature_left;
		}

		double TM_tright() const
		{
			return _TM_tright;
		}

		void TM_tright(double tm_temperature_right)
		{
			changedStatus = false;
			_TM_tright = tm_temperature_right;
		}

		int n_temperatures() const
		{
			return _n_temperatures;
		}

		void n_temperatures(int n_temperatures)
		{
			changedStatus = false;
			_n_temperatures = n_temperatures;
		}

		double tleft() const
		{
			return _tleft;
		}

		void tleft(double temperature_left)
		{
			changedStatus = false;
			_tleft = temperature_left;
		}

		double tright() const
		{
			return _tright;
		}

		void tright(double temperature_right)
		{
			changedStatus = false;
			_tright = temperature_right;
		}

		int isotropic() const
		{
			return _isotropic;
		}

		void isotropic(int isotropic)
		{
			changedStatus = false;
			_isotropic = isotropic;
		}

		int fission_type() const
		{
			return _fission_type;
		}

		void fission_type(int fission_type)
		{
			changedStatus = false;
			_fission_type = fission_type;
		}

		int iName() const
		{
			return _iName;
		}

		void iName(int names_def_number)
		{
			changedStatus = false;
			_iName = names_def_number;
		}

		bool getStatus() const
		{
			return changedStatus;
		}
		void fixStatus() const
		{
			changedStatus = true;
		}
		void freeStatus() const
		{
			changedStatus = false;
		}

		friend std::ostream& operator<<(std::ostream& os, const NuclearData& h)
		{
			os
				<< "_iConfig: " << h._iConfig
				<< " _mTM: " << h._iTM
				<< " _mResonance: " << h._iResonance
				<< " _mUnresolved: " << h._iUrr
				<< " _nTM_temperatures: " << h._nTM_temperatures
				<< " _TM_temperature_left: " << h._TM_tleft
				<< " _TM_temperature_right: " << h._TM_tright
				<< " _n_temperatures: " << h._n_temperatures
				<< " _temperature_left: " << h._tleft
				<< " _temperature_right: " << h._tright
				<< " _isotropic: " << h._isotropic
				<< " _fission_type: " << h._fission_type
				<< " _names_def_number: " << h._iName
				<< " status: " << h.changedStatus;
			return os;
		}

	private:
		int _iConfig; /* 20 bits */
		int _iTM; /* 2 bits */
		int _iResonance; /* 2 bits */
		int _iUrr; /* 1 bit  */
		int _nTM_temperatures; /* 2 bits */
		double _TM_tleft; /* 8 bytes */
		double _TM_tright; /* 8 bytes */
		int _n_temperatures; /* 2 bits */
		double _tleft; /* 8 bytes */
		double _tright; /* 8 bytes */
		int _isotropic; /* 1 bit  */
		int _fission_type; /* 2 bit  */
		int _iName; /* 31 bits */ // Only positive values

		mutable bool changedStatus; // For check was changed
	};

} // namespace DataLib


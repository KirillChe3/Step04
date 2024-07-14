// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CStep04App : public AcRxArxApp {

public:
	CStep04App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
	// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member 
	// function of the CStep04App class.
	// The function should take no arguments and return nothing.
	//
	// NOTE: ACED_ARXCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid and
	// have arguments to define context and command mechanism.
	
	// ACED_ARXCOMMAND_ENTRY_AUTO(classname, group, globCmd, locCmd, cmdFlags, UIContext)
	// ACED_ARXCOMMAND_ENTRYBYID_AUTO(classname, group, globCmd, locCmdId, cmdFlags, UIContext)
	// only differs that it creates a localized name using a string in the resource file
	//   locCmdId - resource ID for localized command

	// Modal Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL)

	static void ADSKStep04ADDENTRY() {
		try
		{
			ACHAR strId[132];
			if (acedGetString(0, L"\nEnter employee name:", strId) != RTNORM) {
				return;
			}

			AcDbDictionary* pNOD;
			if (acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNOD, AcDb::kForRead) != Acad::eOk) {
				acutPrintf(L"Cannot open the NOD! Terminate...");
				return;
			}

			AcDbObjectId idO;
			AcDbDictionary* pEmployeeDict = NULL;
			if (pNOD->getAt(L"ADSK_EMPLOYEE_DICTIONARY", idO) == Acad::eKeyNotFound) {
				if (pNOD->upgradeOpen() != Acad::eOk) {
					acutPrintf(L"\nCannot open NOD");
					pNOD->close();
					return;
				}

				pEmployeeDict = new AcDbDictionary;
				if (pNOD->setAt(L"ADSK_EMPLOYEE_DICTIONARY", pEmployeeDict, idO) != Acad::eOk) {
					acutPrintf(L"\nCannot add our dictionary to AutoCAD NOD");
					delete pEmployeeDict;
					pNOD->close();
					return;
				}
			}
			else
			{
				AcDbObject* pO;
				if (acdbOpenAcDbObject(pO, idO, AcDb::kForWrite) != Acad::eOk) {
					acutPrintf(L"\nCannot open the object");
					pNOD->close();
					return;
				}
				if ((pEmployeeDict = AcDbDictionary::cast(pO)) == NULL) {
					acutPrintf(L"\nEntry found in the NOD, but it's not the dictionary");
					pO->close();
					pNOD->close();
					return;
				}
			}
			pNOD->close();
			if (pEmployeeDict->getAt(strId, idO) == Acad::eOk) {
				acutPrintf(L"\nEmployee is already registered");
				pEmployeeDict->close();
				return;
			}

			AcDbXrecord* pEmployeeEntry = new AcDbXrecord;
			if (pEmployeeDict->setAt(strId, pEmployeeEntry, idO) != Acad::eOk) {
				acutPrintf(L"\nCannot add new employee to the dictionary");
				delete pEmployeeEntry;
				pEmployeeDict->close();
				return;
			}
			pEmployeeEntry->close();
			pEmployeeDict->close();
		}
		catch (...)
		{
		}
	}

	static void ADSKStep04LISTENTRY() {
		try
		{
			AcDbDictionary* pNOD;
			if (acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNOD, AcDb::kForRead) != Acad::eOk) {
				acutPrintf(L"\nCannot open the NOD. Terminated...");
				return;
			}
			AcDbObjectId idO;
			AcDbObject* pO;
			if (pNOD->getAt(L"ADSK_EMPLOYEE_DICTIONARY", idO) != Acad::eOk) {
				acutPrintf(L"\nError of finding dictionary");
				pNOD->close();
				return;
			}
			if (acdbOpenAcDbObject(pO, idO, AcDb::kForRead) != Acad::eOk) {
				acutPrintf(L"\nCannot open the object");
				pNOD->close();
				return;
			}

			AcDbDictionary* pEmployeeDict;
			if ((pEmployeeDict = AcDbDictionary::cast(pO)) == NULL) {
				acutPrintf(L"\nEntry found in the NOD, but it,s not the dictionary");
				pO->close();
				pNOD->close();
				return;
			}
			pNOD->close();

			AcDbDictionaryIterator* pIter;
			if ((pIter = pEmployeeDict->newIterator()) != NULL) {
				for (; !(pIter->done()); pIter->next()) {
					acutPrintf(L"\nEmployee:%s", pIter->name());
				}
				delete pIter;
			}
			pEmployeeDict->close();
		}
		catch (...)
		{
		}
	}

	static void ADSKStep04REMOVEENTRY() {
		try
		{
			AcString strId;
			if (acedGetString(0, L"Enter employee name:", strId) != RTNORM) {
				return;
			}

			AcDbDictionary* pNOD;
			if ((acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNOD, AcDb::kForRead)) != Acad::eOk) {
				acutPrintf(L"Cannot open the NOD. Terminated...");
				return;
			}

			AcDbObjectId idO;
			AcDbObject* pO;
			if ((pNOD->getAt(L"ADSK_EMPLOYEE_DICTIONARY", idO)) != Acad::eOk) {
				acutPrintf(L"\nNo dictionary, no entry to remove");
				pNOD->close();
				return;
			}

			if (acdbOpenObject(pO, idO, AcDb::kForRead) != Acad::eOk) {
				acutPrintf(L"\nCannot open the object");
				pNOD->close();
				return;
			}

			AcDbDictionary* pEmployeeDict;

			if ((pEmployeeDict = AcDbDictionary::cast(pO)) == NULL) {
				acutPrintf(L"\nEntry found, but it's not dictionary");
				pO->close();
				pNOD->close();
				return;
			}
			pNOD->close();

			if (pEmployeeDict->getAt(strId, idO) != Acad::eOk) {
				acutPrintf(L"\nEntry not found");
				pEmployeeDict->close();
				return;
			}
			pEmployeeDict->close();

			if ((acdbOpenAcDbObject(pO, idO, AcDb::kForWrite)) != Acad::eOk) {
				acutPrintf(L"\nEntry cannot open for write");
				return;
			}

			pO->erase();
			pO->close();
		}
		catch (...)
		{
		}
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep04App)


ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKStep04, ADDENTRY, ADDENTRY, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKStep04, LISTENTRY, LISTENTRY, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKStep04, REMOVEENTRY, REMOVEENTRY, ACRX_CMD_TRANSPARENT, NULL)
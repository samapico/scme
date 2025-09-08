#ifndef INC_UiGlobal_H
#define INC_UiGlobal_H

///////////////////////////////////////////////////////////////////////////

#ifdef SCME_UI_EXPORT_SYMBOLS //defined in scme.vcxproj
#define SCME_UI_DLL __declspec(dllexport)
#else
#define SCME_UI_DLL __declspec(dllimport)
#endif

#include <Global.h>


///////////////////////////////////////////////////////////////////////////

namespace SCME {


} // End namespace SCME

#endif // INC_UiGlobal_H

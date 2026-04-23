/**
 * @file OcctDfmCapi.cpp
 * @brief C ABI wrapper around the DFM SDK for C# P/Invoke callers.
 */

#include "OcctDfmCapi.h"

#include "OcctDfmSession.h"

#include <Quantity_Color.hxx>

#include <codecvt>
#include <cwchar>
#include <locale>
#include <string>

namespace
{
struct DfmCapiSession
{
    OcctDfmSession session;
    std::string wrapperError;
};

DfmCapiSession* fromHandle(OcctDfmHandle handle)
{
    return static_cast<DfmCapiSession*>(handle);
}

void clearWrapperError(DfmCapiSession* session)
{
    if (session != nullptr)
    {
        session->wrapperError.clear();
    }
}

void setWrapperError(DfmCapiSession* session, const std::string& errorText)
{
    if (session != nullptr)
    {
        session->wrapperError = errorText;
    }
}

std::string getEffectiveLastError(const DfmCapiSession* session)
{
    if (session == nullptr)
    {
        return "session handle is null";
    }

    if (!session->wrapperError.empty())
    {
        return session->wrapperError;
    }

    return session->session.getLastError();
}

std::string utf8FromWide(const wchar_t* text)
{
    if (text == nullptr)
    {
        return std::string();
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(text);
}

std::wstring wideFromUtf8(const std::string& text)
{
    if (text.empty())
    {
        return std::wstring();
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(text);
}

int copyWideString(const std::wstring& text, wchar_t* buffer, int bufferCount)
{
    if (buffer == nullptr || bufferCount <= 0)
    {
        return 0;
    }

    const size_t requiredCount = text.size() + 1;
    if (requiredCount > static_cast<size_t>(bufferCount))
    {
        return 0;
    }

    std::wmemcpy(buffer, text.c_str(), text.size());
    buffer[text.size()] = L'\0';
    return 1;
}

template <typename Fn>
int invokeBool(DfmCapiSession* session, Fn&& fn)
{
    if (session == nullptr)
    {
        return 0;
    }

    clearWrapperError(session);
    try
    {
        return fn() ? 1 : 0;
    }
    catch (const std::exception& exception)
    {
        setWrapperError(session, exception.what());
        return 0;
    }
    catch (...)
    {
        setWrapperError(session, "unknown native exception");
        return 0;
    }
}
} // namespace

OcctDfmHandle OCCTDFM_CALL OcctDfm_CreateSession(void)
{
    try
    {
        return new DfmCapiSession();
    }
    catch (...)
    {
        return nullptr;
    }
}

void OCCTDFM_CALL OcctDfm_DestroySession(OcctDfmHandle handle)
{
    delete fromHandle(handle);
}

int OCCTDFM_CALL OcctDfm_InitializeLoggingW(OcctDfmHandle handle, const wchar_t* logDirPath)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return 0;
    }

    const std::string utf8Path =
      (logDirPath != nullptr) ? utf8FromWide(logDirPath) : std::string("logs");
    return invokeBool(session, [&]() { return session->session.initializeLogging(utf8Path); });
}

int OCCTDFM_CALL OcctDfm_LoadTargetStepFileW(OcctDfmHandle handle, const wchar_t* stepFilePath)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return 0;
    }

    if (stepFilePath == nullptr || stepFilePath[0] == L'\0')
    {
        setWrapperError(session, "step file path is empty");
        return 0;
    }

    const std::string utf8Path = utf8FromWide(stepFilePath);
    return invokeBool(session, [&]() { return session->session.loadTargetStepFile(utf8Path); });
}

int OCCTDFM_CALL OcctDfm_LoadDfmReportFromFileW(OcctDfmHandle handle, const wchar_t* reportFilePath)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return 0;
    }

    if (reportFilePath == nullptr || reportFilePath[0] == L'\0')
    {
        setWrapperError(session, "report file path is empty");
        return 0;
    }

    const std::string utf8Path = utf8FromWide(reportFilePath);
    return invokeBool(session, [&]() { return session->session.loadDfmReportFromFile(utf8Path); });
}

int OCCTDFM_CALL OcctDfm_LoadDfmReportFromJsonW(OcctDfmHandle handle, const wchar_t* jsonText)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return 0;
    }

    if (jsonText == nullptr)
    {
        setWrapperError(session, "json text is null");
        return 0;
    }

    const std::string utf8Json = utf8FromWide(jsonText);
    return invokeBool(session, [&]() { return session->session.loadDfmReportFromJson(utf8Json); });
}

void OCCTDFM_CALL OcctDfm_Clear(OcctDfmHandle handle)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return;
    }

    clearWrapperError(session);
    session->session.clear();
}

int OCCTDFM_CALL OcctDfm_HasDfmReport(OcctDfmHandle handle)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return 0;
    }

    return session->session.hasDfmReport() ? 1 : 0;
}

int OCCTDFM_CALL OcctDfm_IsProcessable(OcctDfmHandle handle)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return 0;
    }

    return session->session.isDfmProcessable() ? 1 : 0;
}

int OCCTDFM_CALL OcctDfm_GetFaceSeverityW(OcctDfmHandle handle, const wchar_t* faceId)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return -1;
    }

    if (faceId == nullptr || faceId[0] == L'\0')
    {
        setWrapperError(session, "face id is empty");
        return -1;
    }

    clearWrapperError(session);
    const std::string utf8FaceId = utf8FromWide(faceId);
    return static_cast<int>(session->session.getFaceSeverity(utf8FaceId));
}

int OCCTDFM_CALL OcctDfm_GetFaceColorW(OcctDfmHandle handle,
                                       const wchar_t* faceId,
                                       OcctDfmColor* outColor)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr || outColor == nullptr)
    {
        return 0;
    }

    if (faceId == nullptr || faceId[0] == L'\0')
    {
        setWrapperError(session, "face id is empty");
        return 0;
    }

    clearWrapperError(session);
    const std::string utf8FaceId = utf8FromWide(faceId);
    const Quantity_Color color = session->session.getFaceDisplayColor(utf8FaceId);
    outColor->red = static_cast<float>(color.Red());
    outColor->green = static_cast<float>(color.Green());
    outColor->blue = static_cast<float>(color.Blue());
    return 1;
}

int OCCTDFM_CALL OcctDfm_GetLastErrorLengthW(OcctDfmHandle handle)
{
    return static_cast<int>(wideFromUtf8(getEffectiveLastError(fromHandle(handle))).size());
}

int OCCTDFM_CALL OcctDfm_CopyLastErrorW(OcctDfmHandle handle, wchar_t* buffer, int bufferCount)
{
    return copyWideString(wideFromUtf8(getEffectiveLastError(fromHandle(handle))),
                          buffer,
                          bufferCount);
}

int OCCTDFM_CALL OcctDfm_GetVisualizationJsonLengthW(OcctDfmHandle handle)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return 0;
    }

    clearWrapperError(session);
    try
    {
        return static_cast<int>(wideFromUtf8(session->session.buildVisualizationJson()).size());
    }
    catch (const std::exception& exception)
    {
        setWrapperError(session, exception.what());
        return 0;
    }
    catch (...)
    {
        setWrapperError(session, "unknown native exception");
        return 0;
    }
}

int OCCTDFM_CALL OcctDfm_CopyVisualizationJsonW(OcctDfmHandle handle,
                                                wchar_t* buffer,
                                                int bufferCount)
{
    DfmCapiSession* session = fromHandle(handle);
    if (session == nullptr)
    {
        return 0;
    }

    clearWrapperError(session);
    try
    {
        return copyWideString(wideFromUtf8(session->session.buildVisualizationJson()),
                              buffer,
                              bufferCount);
    }
    catch (const std::exception& exception)
    {
        setWrapperError(session, exception.what());
        return 0;
    }
    catch (...)
    {
        setWrapperError(session, "unknown native exception");
        return 0;
    }
}

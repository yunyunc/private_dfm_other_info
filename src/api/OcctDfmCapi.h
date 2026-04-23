/**
 * @file OcctDfmCapi.h
 * @brief C ABI exported from the DFM SDK for C# P/Invoke callers.
 */
#pragma once

#ifdef _WIN32
#  ifdef OCCTDFM_SDK_EXPORTS
#    define OCCTDFM_C_API __declspec(dllexport)
#  else
#    define OCCTDFM_C_API __declspec(dllimport)
#  endif
#  define OCCTDFM_CALL __cdecl
#else
#  define OCCTDFM_C_API
#  define OCCTDFM_CALL
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Opaque DFM session handle for C ABI callers.
 */
typedef void* OcctDfmHandle;

/**
 * @brief Severity values returned by the C ABI.
 */
enum OcctDfmSeverity
{
    OCCT_DFM_SEVERITY_NONE = 0,
    OCCT_DFM_SEVERITY_YELLOW = 1,
    OCCT_DFM_SEVERITY_RED = 2
};

/**
 * @brief RGB color payload returned by the C ABI.
 */
typedef struct OcctDfmColor
{
    float red;
    float green;
    float blue;
} OcctDfmColor;

/**
 * @brief Creates a new C ABI DFM session.
 * @return Opaque handle, or null on failure.
 */
OCCTDFM_C_API OcctDfmHandle OCCTDFM_CALL OcctDfm_CreateSession(void);

/**
 * @brief Destroys a C ABI DFM session.
 * @param handle Opaque session handle.
 */
OCCTDFM_C_API void OCCTDFM_CALL OcctDfm_DestroySession(OcctDfmHandle handle);

/**
 * @brief Initializes logging for the session.
 * @param handle Opaque session handle.
 * @param logDirPath UTF-16 log directory path.
 * @return 1 on success, 0 on failure.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_InitializeLoggingW(OcctDfmHandle handle,
                                                          const wchar_t* logDirPath);

/**
 * @brief Loads a STEP file used as the target CAD shape.
 * @param handle Opaque session handle.
 * @param stepFilePath UTF-16 STEP file path.
 * @return 1 on success, 0 on failure.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_LoadTargetStepFileW(OcctDfmHandle handle,
                                                           const wchar_t* stepFilePath);

/**
 * @brief Loads a DFM report from a JSON file.
 * @param handle Opaque session handle.
 * @param reportFilePath UTF-16 JSON file path.
 * @return 1 on success, 0 on failure.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_LoadDfmReportFromFileW(OcctDfmHandle handle,
                                                              const wchar_t* reportFilePath);

/**
 * @brief Loads a DFM report from in-memory JSON text.
 * @param handle Opaque session handle.
 * @param jsonText UTF-16 JSON text.
 * @return 1 on success, 0 on failure.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_LoadDfmReportFromJsonW(OcctDfmHandle handle,
                                                              const wchar_t* jsonText);

/**
 * @brief Clears current shape and report state.
 * @param handle Opaque session handle.
 */
OCCTDFM_C_API void OCCTDFM_CALL OcctDfm_Clear(OcctDfmHandle handle);

/**
 * @brief Returns whether a DFM report is loaded.
 * @param handle Opaque session handle.
 * @return 1 when a report is loaded, otherwise 0.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_HasDfmReport(OcctDfmHandle handle);

/**
 * @brief Returns whether the current part is processable.
 * @param handle Opaque session handle.
 * @return 1 when processable, otherwise 0.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_IsProcessable(OcctDfmHandle handle);

/**
 * @brief Returns severity for a given face-id.
 * @param handle Opaque session handle.
 * @param faceId UTF-16 face-id string.
 * @return Severity enum value, or -1 on failure.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_GetFaceSeverityW(OcctDfmHandle handle,
                                                        const wchar_t* faceId);

/**
 * @brief Returns display color for a given face-id.
 * @param handle Opaque session handle.
 * @param faceId UTF-16 face-id string.
 * @param outColor Output RGB color.
 * @return 1 on success, 0 on failure.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_GetFaceColorW(OcctDfmHandle handle,
                                                     const wchar_t* faceId,
                                                     OcctDfmColor* outColor);

/**
 * @brief Returns required UTF-16 character count for the last error string.
 * @param handle Opaque session handle.
 * @return Character count excluding null terminator.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_GetLastErrorLengthW(OcctDfmHandle handle);

/**
 * @brief Copies the last error string into a caller-provided UTF-16 buffer.
 * @param handle Opaque session handle.
 * @param buffer Destination buffer.
 * @param bufferCount Buffer capacity in wchar_t, including null terminator.
 * @return 1 on success, 0 on failure.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_CopyLastErrorW(OcctDfmHandle handle,
                                                      wchar_t* buffer,
                                                      int bufferCount);

/**
 * @brief Returns required UTF-16 character count for the visualization JSON.
 * @param handle Opaque session handle.
 * @return Character count excluding null terminator.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_GetVisualizationJsonLengthW(OcctDfmHandle handle);

/**
 * @brief Copies visualization JSON into a caller-provided UTF-16 buffer.
 * @param handle Opaque session handle.
 * @param buffer Destination buffer.
 * @param bufferCount Buffer capacity in wchar_t, including null terminator.
 * @return 1 on success, 0 on failure.
 */
OCCTDFM_C_API int OCCTDFM_CALL OcctDfm_CopyVisualizationJsonW(OcctDfmHandle handle,
                                                               wchar_t* buffer,
                                                               int bufferCount);

#ifdef __cplusplus
}
#endif

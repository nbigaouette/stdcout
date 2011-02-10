#ifndef INC_VERSION_hpp
#define INC_VERSION_hpp

extern const char *build_time;
extern const char *build_sha;
extern const char *build_branch;

extern void log(const char *const format, ...);

inline void Log_Git_Info()
{
    log("##############################################################\n");
    log("# StdCout library git versioning:                            #\n");
    log("#    build_time:   %-41s #\n", build_time);
    log("#    build_sha:    %-41s #\n", build_sha);
    log("#    build_branch: %-41s #\n", build_branch);
    log("##############################################################\n");
}

#endif // INC_VERSION_hpp

// ********** End of file ***************************************

; ---------- cbs.sch schema for consultants's billing system

; ------ data element dictionary

#schema CBS

#dictionary
    CLIENT_NO,       Z, 5
    CLIENT_NAME,     A, 25
    ADDRESS,         A, 25
    CITY,            A, 25
    STATE,           A, 2
    ZIP,             N, 8
    PHONE,           N, 10, "(___)___-____"
    AMT_DUE,         C, 8,  "$______.__"
    PROJECT_NO,      Z, 5
    PROJECT_NAME,    A, 25
    AMT_EXPENDED,    C, 8,  "$______.__"
    CONSULTANT_NO,   Z, 5
    CONSULTANT_NAME, A, 25
    RATE,            C, 5,  "$___.__"
    PAYMENT,         C, 8,  "$______.__"
    EXPENSE,         C, 8,  "$______.__"
    HOURS,           N, 2
    DATE_PAID,       D, 6, "__/__/__"
#end dictionary

;---- file specifications

#file CLIENTS
    CLIENT_NO
    CLIENT_NAME
    ADDRESS
    CITY
    STATE
    ZIP
    PHONE
    AMT_DUE
    DATE_PAID
#end file

#file PROJECTS
    PROJECT_NO
    PROJECT_NAME
    AMT_EXPENDED
    CLIENT_NO
#end file

#file CONSULTANTS
    CONSULTANT_NO
    CONSULTANT_NAME
#end file

#file ASSIGNMENTS
    CONSULTANT_NO
    PROJECT_NO
    RATE
#end file

; ----- index specifications

#key CLIENTS CLIENT_NO
#key PROJECTS PROJECT_NO
#key CONSULTANTS CONSULTANT_NO
#key ASSIGNMENTS CONSULTANT_NO, PROJECT_NO
#key ASSIGNMENTS CONSULTANT_NO
#key ASSIGNMENTS PROJECT_NO

#end schema CBS

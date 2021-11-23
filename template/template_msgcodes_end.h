
        /* Last value (DO NOT DELETE) */
        NUMBER_OF_FRAMEWORK_MSG_CODES
};
BUILD_ASSERT(sizeof(enum FwkMsgCodeEnum) <= sizeof(FwkMsgCode_t),
             "Too many message codes");

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_MSG_CODES_H__ */

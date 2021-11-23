
        /* Reserved for framework (DO NOT DELETE, and it must be LAST) */
        __FRAMEWORK_MAX_MSG_RECEIVERS
};
BUILD_ASSERT(__FRAMEWORK_MAX_MSG_RECEIVERS <= CONFIG_FWK_MAX_MSG_RECEIVERS,
             "Adjust number of message receivers in Kconfig");

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_IDS__ */

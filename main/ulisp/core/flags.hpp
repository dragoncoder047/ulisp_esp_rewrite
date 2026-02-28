typedef struct {
    bool printing_readably : 1;
    bool return_was_called : 1;
    bool user_interrupt : 1;
    bool want_to_exit_editor : 1;
    bool library_was_loaded : 1;
    bool dont_escape : 1;
    bool disable_echo : 1;
    bool inside_error_handler : 1;
    bool should_tail_call : 1;
} flags_t;

extern flags_t Flags;

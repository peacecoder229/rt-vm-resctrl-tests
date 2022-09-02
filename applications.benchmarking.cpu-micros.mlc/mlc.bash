_mlc()
{
    local cur prev mode matrix_opts mode_opts
    COMPREPLY=()
    prev="${COMP_WORDS[COMP_CWORD-1]}"

    cur="${COMP_WORDS[COMP_CWORD]}"
    mlc_name="${COMP_WORDS[0]}"

    mode="${COMP_WORDS[1]}"
    matrix_opts=$(./${mlc_name} -h 2>&1 | awk '/^\s*.\/mlc --.*?print/ {print $0}' | cut -d" " -f2)
    mode_opts=$(./${mlc_name} -h 2>&1 | awk '/^\s*.\/mlc '${mode}'.*?\[/ {print $0}' | cut -d" " -f3- | tr -d '[]')

    case $COMP_CWORD in
        1)
            COMPREPLY=( $(compgen -W "${matrix_opts}" -- ${cur}) )
            return 0
            ;;
        2|*)
            COMPREPLY=( $(compgen -W "${mode_opts}" -- ${cur}) )
            return 0
            ;;
    esac
}
complete -F _mlc ./mlc ./mlc_internal ./mlc_numa ./mlc_avx512 mlc mlc_internal mlc_numa mlc_avx512

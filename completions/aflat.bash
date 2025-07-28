_aflat_completion() {
    local cur prev commands opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    commands="make build run test add file module install update clean"
    opts="-o --output -c --config -n --name -d --debug -q --quiet -t --trace-alerts -U --update-deps -K --clean-deps -C --clean-cache -N --no-cache -j --jobs -L --lib -h --help"
    if [[ ${cur} == -* ]]; then
        COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
        return 0
    fi
    if [[ ${COMP_CWORD} -eq 1 ]]; then
        COMPREPLY=( $(compgen -W "${commands}" -- ${cur}) )
        return 0
    fi
}
complete -F _aflat_completion aflat

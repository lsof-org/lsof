# Contributing

You can contribute to lsof via pull requests at [GitHub](https://github.com/lsof-org/lsof).

## Commit message

If a change is dialect specific, use `[dialect]` as the prefix of the
header of the commit log like:

	[linux] compile with -Wall option
	[linux] delete unused variables
	[freebsd] cirrus-ci: disabled

If no `[dialect]` prefix is given to a commit, the change may have an impact
across dialects.

## Code style

C sources should be formatted by `clang-format`, e.g.:

```shell
clang-format -i lib/dialcets/linux/dsock.c
# or
git-clang-format
```

The formatter may not function properly in some corner cases. You will have to
rewrite the code to make it happy.

Use as few #if/#else/#endif constructs as possible, even at
the cost of nearly-duplicate code.

When #if/#else/#endif constructs are necessary:

Use the form

```c
#if defined(s<symbol>)
```

in preference to

```c
#ifdef <symbol>
```

to allow easier addition of tests to the #if.

- Indent them to signify their level -- e.g.,

```c
#if		/* level one */
#  if		/* level two */
#  endif	/* level two */
#else		/* level one */
#endif		/* level one */
```

Use ANSI standard comments on #else and #endif statements.

## Testing

There are two test mechanism, the original one by Via Abell and a script-based
one by Masatake YAMATO. About the original test mechanism, see tests/00README.
About the script-based test mechanism, see tests/case-00-hello.bash and
check.bash.

Your pull request should pass all CI checks. If necessary, you can modify testcases.
0. Install ksh
========================================================================

The script for making an archive needs ksh.


1. Implement "make dist" for your favorite dialect
========================================================================

See https://github.com/lsof-org/lsof/pull/131 how @masatake does for
linux dialect.

Merge the changes.


2. Update the version and release date in the source tree if you need
========================================================================

See git log 7412e7445377c0f629c1ecb2eecfd14255935c7d.

The version number has following form MAJOR.MINOR.MICRO.  When making a
release, update the version number.  If the change from the last release
is small, increment MICRO. If it is large, increment MINOR.

Merge the changes.

If you just want to make a release for a version already released in
another dialect, you don't need this step. Release the version number.

e.g.

     conditions and situations:
     @masatake already released 4.94.0 for linux.
     No user visible change is committed to our git repo.
     You may want to make the same release but for dialect freebsd.

     In this case, you can use 4.94.0 as the version number.  If the
     condition doesn't meet, use 4.94.1 (or 4.95.0).

3. Put a tag with Git
=======================================================

NOTE: this step is an optional if the commit for releasing is already
tagged via another dialect.

The tag you will put should have the same name as the version given in
the step 2.

Do "git tag" like::

  $ git tag 4.95.0-this-is-an-example
  $ git push upstream 4.95.0-this-is-an-example

If you took a mistake, you can delete the tag like::

  $ git tag -d 4.95.0-this-is-an-example
  $ git push upstream :4.95.0-this-is-an-example

4. Make the source archive for the release
=======================================================
::

   $ ./Configure [dialect]
   $ make dist

The archive is put at support/ directory.

Checking whether you can build a lsof executable from the archive is
good idea.

e.g.
::

   $ cp support/support/lsof_4.94.0.linux.tar.bz2
   $ cd /tmp
   $ tar jxvf lsof_4.94.0.linux.tar.bz2
   $ cd lsof_4.94.0.linux
   $ ./Configure linux
   $ make
   $ make check (if your dialect support the target)

5. Visit https://github.com/lsof-org/lsof/releases
========================================================================

5.1 Click [Draft new release]
------------------------------------------------------------------------

5.2 Fill the fields
------------------------------------------------------------------------

5.2.1 Choose a tag
........................................................................

Choose with the tag given in the step 3.

5.2.2 Fill "Release title"
........................................................................

Use the following form lsof-${theVersionNumber}-${dialect}.

e.g. lsof-4.94.0-linux

5.2.3 Fill "Describe this release"
........................................................................

Copy and paste the changes described in 00DIST since the last release in
your dialect.

5.2.4 Put the source archive generated step in 4.
........................................................................

Click "Attach binaries by dropping them here or selecting them. ", then
specify the archive file.

5.2.5 Check "This is a pre-release"
........................................................................

5.2.6 Click [Publish release]
........................................................................

6. Verify the release
========================================================================

After step 5.2.6, the browser may show the page for the release. You can
re-read the description and the source code archives. You can verify
what you did here.

If you find a fault or something, click the [Edit] in the page. You can
update the pre-release.

If you convince it is ready, click the [Edit] in the page, remove the
check at "This is a pre-release", then [Publish release]. The new
version is available at https://github.com/lsof-org/lsof/releases.

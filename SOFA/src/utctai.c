#include "sofa.h"
#include "sofam.h"

int iauUtctai(double utc1, double utc2, double *tai1, double *tai2)
/*
**  - - - - - - - - - -
**   i a u U t c t a i
**  - - - - - - - - - -
**
**  Time scale transformation:  Coordinated Universal Time, UTC, to
**  International Atomic Time, TAI.
**
**  This function is part of the International Astronomical Union's
**  SOFA (Standards of Fundamental Astronomy) software collection.
**
**  Status:  canonical.
**
**  Given:
**     utc1,utc2  double   UTC as a 2-part quasi Julian Date (Notes 1-4)
**
**  Returned:
**     tai1,tai2  double   TAI as a 2-part Julian Date (Note 5)
**
**  Returned (function value):
**                int      status: +1 = dubious year (Note 3)
**                                  0 = OK
**                                 -1 = unacceptable date
**
**  Notes:
**
**  1) utc1+utc2 is quasi Julian Date (see Note 2), apportioned in any
**     convenient way between the two arguments, for example where utc1
**     is the Julian Day Number and utc2 is the fraction of a day.
**
**  2) JD cannot unambiguously represent UTC during a leap second unless
**     special measures are taken.  The convention in the present
**     function is that the JD day represents UTC days whether the
**     length is 86399, 86400 or 86401 SI seconds.  In the 1960-1972 era
**     there were smaller jumps (in either direction) each time the
**     linear UTC(TAI) expression was changed, and these "mini-leaps"
**     are also included in the SOFA convention.
**
**  3) The warning status "dubious year" flags UTCs that predate the
**     introduction of the time scale or that are too far in the future
**     to be trusted.  See iauDat for further details.
**
**  4) The function iauDtf2d converts from calendar date and time of day
**     into 2-part Julian Date, and in the case of UTC implements the
**     leap-second-ambiguity convention described above.
**
**  5) The returned TAI1,TAI2 are such that their sum is the TAI Julian
**     Date.
**
**  Called:
**     iauJd2cal    JD to Gregorian calendar
**     iauDat       delta(AT) = TAI-UTC
**     iauCal2jd    Gregorian calendar to JD
**
**  References:
**
**     McCarthy, D. D., Petit, G. (eds.), IERS Conventions (2003),
**     IERS Technical Note No. 32, BKG (2004)
**
**     Explanatory Supplement to the Astronomical Almanac,
**     P. Kenneth Seidelmann (ed), University Science Books (1992)
**
**  This revision:  2021 May 11
**
**  SOFA release 2021-05-12
**
**  Copyright (C) 2021 IAU SOFA Board.  See notes at end.
**
*/
{
   int big1;
   int iy, im, id, j, iyt, imt, idt;
   double u1, u2, fd, dat0, dat12, w, dat24, dlod, dleap, z1, z2, a2;


/* Put the two parts of the UTC into big-first order. */
   big1 = ( fabs(utc1) >= fabs(utc2) );
   if ( big1 ) {
      u1 = utc1;
      u2 = utc2;
   } else {
      u1 = utc2;
      u2 = utc1;
   }

/* Get TAI-UTC at 0h today. */
   j = iauJd2cal(u1, u2, &iy, &im, &id, &fd);
   if ( j ) return j;
   j = iauDat(iy, im, id, 0.0, &dat0);
   if ( j < 0 ) return j;

/* Get TAI-UTC at 12h today (to detect drift). */
   j = iauDat(iy, im, id, 0.5, &dat12);
   if ( j < 0 ) return j;

/* Get TAI-UTC at 0h tomorrow (to detect jumps). */
   j = iauJd2cal(u1+1.5, u2-fd, &iyt, &imt, &idt, &w);
   if ( j ) return j;
   j = iauDat(iyt, imt, idt, 0.0, &dat24);
   if ( j < 0 ) return j;

/* Separate TAI-UTC change into per-day (DLOD) and any jump (DLEAP). */
   dlod = 2.0 * (dat12 - dat0);
   dleap = dat24 - (dat0 + dlod);

/* Remove any scaling applied to spread leap into preceding day. */
   fd *= (DAYSEC+dleap)/DAYSEC;

/* Scale from (pre-1972) UTC seconds to SI seconds. */
   fd *= (DAYSEC+dlod)/DAYSEC;

/* Today's calendar date to 2-part JD. */
   if ( iauCal2jd(iy, im, id, &z1, &z2) ) return -1;

/* Assemble the TAI result, preserving the UTC split and order. */
   a2 = z1 - u1;
   a2 += z2;
   a2 += fd + dat0/DAYSEC;
   if ( big1 ) {
      *tai1 = u1;
      *tai2 = a2;
   } else {
      *tai1 = a2;
      *tai2 = u1;
   }

/* Status. */
   return j;

/* Finished. */

/*----------------------------------------------------------------------
**
**  Copyright (C) 2021
**  Standards Of Fundamental Astronomy Board
**  of the International Astronomical Union.
**
**  =====================
**  SOFA Software License
**  =====================
**
**  NOTICE TO USER:
**
**  BY USING THIS SOFTWARE YOU ACCEPT THE FOLLOWING SIX TERMS AND
**  CONDITIONS WHICH APPLY TO ITS USE.
**
**  1. The Software is owned by the IAU SOFA Board ("SOFA").
**
**  2. Permission is granted to anyone to use the SOFA software for any
**     purpose, including commercial applications, free of charge and
**     without payment of royalties, subject to the conditions and
**     restrictions listed below.
**
**  3. You (the user) may copy and distribute SOFA source code to others,
**     and use and adapt its code and algorithms in your own software,
**     on a world-wide, royalty-free basis.  That portion of your
**     distribution that does not consist of intact and unchanged copies
**     of SOFA source code files is a "derived work" that must comply
**     with the following requirements:
**
**     a) Your work shall be marked or carry a statement that it
**        (i) uses routines and computations derived by you from
**        software provided by SOFA under license to you; and
**        (ii) does not itself constitute software provided by and/or
**        endorsed by SOFA.
**
**     b) The source code of your derived work must contain descriptions
**        of how the derived work is based upon, contains and/or differs
**        from the original SOFA software.
**
**     c) The names of all routines in your derived work shall not
**        include the prefix "iau" or "sofa" or trivial modifications
**        thereof such as changes of case.
**
**     d) The origin of the SOFA components of your derived work must
**        not be misrepresented;  you must not claim that you wrote the
**        original software, nor file a patent application for SOFA
**        software or algorithms embedded in the SOFA software.
**
**     e) These requirements must be reproduced intact in any source
**        distribution and shall apply to anyone to whom you have
**        granted a further right to modify the source code of your
**        derived work.
**
**     Note that, as originally distributed, the SOFA software is
**     intended to be a definitive implementation of the IAU standards,
**     and consequently third-party modifications are discouraged.  All
**     variations, no matter how minor, must be explicitly marked as
**     such, as explained above.
**
**  4. You shall not cause the SOFA software to be brought into
**     disrepute, either by misuse, or use for inappropriate tasks, or
**     by inappropriate modification.
**
**  5. The SOFA software is provided "as is" and SOFA makes no warranty
**     as to its use or performance.   SOFA does not and cannot warrant
**     the performance or results which the user may obtain by using the
**     SOFA software.  SOFA makes no warranties, express or implied, as
**     to non-infringement of third party rights, merchantability, or
**     fitness for any particular purpose.  In no event will SOFA be
**     liable to the user for any consequential, incidental, or special
**     damages, including any lost profits or lost savings, even if a
**     SOFA representative has been advised of such damages, or for any
**     claim by any third party.
**
**  6. The provision of any version of the SOFA software under the terms
**     and conditions specified herein does not imply that future
**     versions will also be made available under the same terms and
**     conditions.
*
**  In any published work or commercial product which uses the SOFA
**  software directly, acknowledgement (see www.iausofa.org) is
**  appreciated.
**
**  Correspondence concerning SOFA software should be addressed as
**  follows:
**
**      By email:  sofa@ukho.gov.uk
**      By post:   IAU SOFA Center
**                 HM Nautical Almanac Office
**                 UK Hydrographic Office
**                 Admiralty Way, Taunton
**                 Somerset, TA1 2DN
**                 United Kingdom
**
**--------------------------------------------------------------------*/
}

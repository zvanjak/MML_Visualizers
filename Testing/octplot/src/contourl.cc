/* Contour lines for function evaluated on a grid.
  Copyright (C) 2004 Shai Ayal

  Adapted to an oct file from the stand alone contourl by Victro Munoz
  Copyright (C) 2004 Victor Munoz

  Based on contour plot routine (plcont.c) in PLPlot package
  http://plplot.org/

  Copyright (C) 1995, 2000, 2001 Maurice LeBrun
  Copyright (C) 2000, 2002 Joao Cardoso
  Copyright (C) 2000, 2001, 2002, 2004  Alan W. Irwin
  Copyright (C) 2004  Andrew Ross

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Library Public License as published
  by the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#include "oct.h"

static Matrix this_contour;
static Matrix contourc;

/**********************************************************************
  cl_add_point(x,y);

  Add a coordinate point (x,y) to this_contour 

**********************************************************************/
void cl_add_point(float x,float y){
  ColumnVector r(2);
  r(0) = x;
  r(1) = y;
  this_contour = this_contour.append(r);
}

/**********************************************************************

  cl_end_contour();

  Adds contents of current contour to contourc.

**********************************************************************/

void cl_end_contour(){
  if (this_contour.cols()>2){
    int new_points = this_contour.cols()-1;
    this_contour(1,0) = new_points;
    contourc = contourc.append(this_contour);
  }
  this_contour = Matrix();
}

/**********************************************************************

  cl_start_contour(flev,x,y);

  Start a new contour, and adds contents of current one to contourc

**********************************************************************/

void cl_start_contour(float flev,float x,float y){
  cl_end_contour();
  this_contour.resize(2,0);
  cl_add_point(flev,flev);
  cl_add_point(x,y);
}

void cl_drawcn(RowVector& X, RowVector& Y,Matrix& Z,float flev,
	   int krow,int kcol,float lastx,float lasty,int startedge,
	    Matrix& ipts){

  int kx = 0, lx = Z.cols()-1, ky = 0, ly = Z.rows()-1;

  float f[4];
  float px[4], py[4], locx[4], locy[4];
  int iedge[4];
  int i, j, k, num, first, inext, kcolnext, krownext;
  
  px[0] = X(krow+1);
  px[1] = X(krow);
  px[2] = X(krow);
  px[3] = X(krow+1);
  py[0] = Y(kcol);
  py[1] = Y(kcol);
  py[2] = Y(kcol+1);
  py[3] = Y(kcol+1);

  f[0] = Z(krow+1,kcol)-flev;
  f[1] = Z(krow,kcol)-flev;
  f[2] = Z(krow,kcol+1)-flev;
  f[3] = Z(krow+1,kcol+1)-flev;
  
  for (int i=0,j=1;i<4;i++,j = (j+1)%4) {
    iedge[i] = (f[i]*f[j]>0.0)?-1:((f[i]*f[j] < 0.0)?1:0);
  }
  
  /* Mark this square as done */
  ipts(krow,kcol) = 1;
  
  /* Check if no contour has been crossed i.e. iedge[i] = -1 */
  if ((iedge[0] == -1) && 
      (iedge[1] == -1) && 
      (iedge[2] == -1) && 
      (iedge[3] == -1)) return;
  
  /* Check if this is a completely flat square - in which case 
     ignore it */
  if ((f[0] == 0.0) && 
      (f[1] == 0.0) && 
      (f[2] == 0.0) && 
      (f[3] == 0.0) ) return;
  
  /* Calculate intersection points */
  num = 0;
  if (startedge < 0) {
    first = 1;
  }
  else {
    locx[num] = lastx;
    locy[num] = lasty;
    num++;
    first = 0;
  }
  
  for (k=0, i = (startedge<0?0:startedge);k<4;k++,i=(i+1)%4) {
    if (i == startedge) continue;
    
    /* If the contour is an edge check it hasn't already been done */
    if (f[i] == 0.0 && f[(i+1)%4] == 0.0) {
      kcolnext = kcol;
      krownext = krow;
      if (i == 0) kcolnext--;
      if (i == 1) krownext--;
      if (i == 2) kcolnext++;
      if (i == 3) krownext++;
      if ((kcolnext < kx) || (kcolnext >= lx) ||
	  (krownext < ky) || (krownext >= ly) ||
	  (ipts(krownext,kcolnext) == 1)) continue;
    }
    if ((iedge[i] == 1) || (f[i] == 0.0)) {
      j = (i+1)%4;
      if (f[i] != 0.0) {
        locx[num] = (px[i]*fabs(f[j])+px[j]*fabs(f[i]))/fabs(f[j]-f[i]);
        locy[num] = (py[i]*fabs(f[j])+py[j]*fabs(f[i]))/fabs(f[j]-f[i]);
      }
      else {
        locx[num] = px[i];
        locy[num] = py[i];
      }
      /* If this is the start of the contour then move to the point */
      if (first == 1) {
        cl_start_contour(flev,locx[num],locy[num]);
        first = 0;
      }
      else { /* Link to the next point on the contour */
        cl_add_point(locx[num],locy[num]);
        /* Need to follow contour into next grid box */
        /* Easy case where contour does not pass through corner */
        if (f[i] != 0.0) {
          kcolnext = kcol;
          krownext = krow;
          inext = (i+2)%4;
          if (i == 0) kcolnext--;
          if (i == 1) krownext--;
          if (i == 2) kcolnext++;
          if (i == 3) krownext++;
          if ((kcolnext >= kx) && (kcolnext < lx) &&
              (krownext >= ky) && (krownext < ly) &&
              (ipts(krownext,kcolnext) == 0)) {
            cl_drawcn(X,Y,Z,flev,krownext,kcolnext,
             locx[num], locy[num], inext, ipts);
          }
        }
        /* Hard case where contour passes through corner *
         * This is still not perfect - it may lose the contour 
         * which won't upset the contour itself (we can find it
         * again later) but might upset the labelling 
         * [which is only relevant for the PLPlot implementation, since
         * we don't worry about labels---for now!]
         */
        else {
          kcolnext = kcol;
          krownext = krow;
          inext = (i+2)%4;
          if (i == 0) {kcolnext--; krownext++;}
          if (i == 1) {krownext--; kcolnext--;}
          if (i == 2) {kcolnext++; krownext--;}
          if (i == 3) {krownext++; kcolnext++;}
          if ((kcolnext >= kx) && (kcolnext < lx) &&
              (krownext >= ky) && (krownext < ly) &&
              (ipts(krownext,kcolnext) == 0)) {
            cl_drawcn(X,Y,Z,flev,krownext, kcolnext,
             locx[num], locy[num], inext, ipts);
          }
          
        }
        if (first == 1) {
          /* Move back to first point */
          cl_start_contour(flev,locx[num],locy[num]);
          first = 0;
        }
        else {
          first = 1;
        }
        num++;
      }
    }
  }
}

void cl_cntr(RowVector& X,RowVector& Y,Matrix& Z,float flev)
{
  Matrix ipts(Z.rows(),Z.cols(),0);

  for (int krow = 0; krow < Z.rows()-1; krow++) {
    for (int kcol = 0; kcol < Z.cols()-1; kcol++) {
      if (ipts(krow,kcol)==0){
        cl_drawcn(X,Y,Z,flev,krow,kcol,0.0,0.0,-2,ipts);
      }
    }
  }
}

DEFUN_DLD (contourl, args, nargout,"")
{
  octave_value_list retval;
  int nargin = args.length ();

  if( nargin !=4 ) {
    error("must have 4 inputs (x,y,z,levels)");
    return retval;
  }

  RowVector X = args(0).row_vector_value();
  RowVector Y = args(1).row_vector_value();
  Matrix Z    = args(2).matrix_value().transpose();
  RowVector L = args(3).row_vector_value();
  
  contourc.resize(2,0);
  for(int i=0;i< L.length() ;i++){
    cl_cntr(X,Y,Z,L(i));
  }
  cl_end_contour();

  retval(0) = contourc;
  return retval;
}

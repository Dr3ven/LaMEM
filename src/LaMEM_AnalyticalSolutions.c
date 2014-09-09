/*
LaMEM
Lithosphere and Mantle Evolution Model

A 3D numerical code that can be used to model geodynamical processes such as
mantle-lithosphere interaction.

Originally developed by:

Boris J.P. Kaus (Uni-Mainz, Germany). 2007-
kaus@uni-mainz.de

Further-development:

Dave May (ETH Zurich, Switzerland). 2009-
dave.may@erdw.ethz.ch

Tobias Baumann (Uni-Mainz, Germany). 2011-
baumann@uni-mainz.de

Anton Popov (Uni-Mainz, Germany). 2011-
popov@uni-mainz.de

$Id:$
$Date:$

This version is compatible with PETSc version 3.3-p2
The code cannot be distributed, used for commercial purposes or handed on,
without the explicit agreement of Boris Kaus.
*/

#include "LaMEM.h"
#include "LaMEM_AnalyticalSolutions.h"
#include "Material.h"


/*
	Set the initial density and viscosity structure if we are performing a benchmark of LaMEM versus an analytical solution.

*/
#undef __FUNCT__
#define __FUNCT__ "LaMEM_Initialize_AnalyticalSolution"
PetscErrorCode LaMEM_Initialize_AnalyticalSolution( UserContext *user, LaMEMVelPressureDA C)
{
	PetscBool 				BenchmarkType, SolCx_y_direction;
	PetscErrorCode 			ierr;
	DAVPElementType 		element_type;
	PetscScalar 			PI=M_PI;
	PetscScalar 			sigma=1, nx=1, nz=1, eta_a=1, eta_b, rho_a, rho_b, x_c, ArcTan_Sharpness;
	PetscInt 				Direction;
	PetscScalar 			***materials_array;
	MaterialsElementDynamic material_data;

	element_type = C->type;


	PetscPrintf(PETSC_COMM_WORLD,"\n\n --------------------------------------------------------------------------------  \n \n");
	PetscPrintf(PETSC_COMM_WORLD,"             PERFORMING BENCHMARK OF LaMEM VERSUS ANALYTICAL SOLUTION              \n");


	/* SolCx benchmark
	 *
	 * density in this benchmark is prescribed as:  rho = -sigma*sin(nz*PI*z)*cos(nx*PI*x)
	 * viscosity varies in x-direction and is eta_a for x<x_c and eta_b for x>x_c
	 *
	 */
	eta_b=1;
	BenchmarkType = PETSC_FALSE;
	ierr 	 = PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
	if (BenchmarkType){


		/* Default parameters */
		x_c   = user->x_left + user->W/2;
		ierr = PetscOptionsGetReal(PETSC_NULL ,"-SolCx_xc",	&x_c	, PETSC_NULL); CHKERRQ(ierr);


		eta_b = user->mumax;

		/* Set some general parameters such as output filename for this benchmark */
		sprintf(user->OutputFile, 		"AnalyticalBenchmark_SolCx");


		/* print info on screen */
		PetscPrintf(PETSC_COMM_WORLD,"  using:                             SolCx                                        \n \n");
		PetscPrintf(PETSC_COMM_WORLD,"   with:      rho   = -1*sin(1*PI*z)*cos(1*PI*x)                                  \n");
		PetscPrintf(PETSC_COMM_WORLD,"              eta   = 1 if x<x_c & eta=eta_b for x>x_c                            \n");
		PetscPrintf(PETSC_COMM_WORLD,"    and:      x_c   = %g     (change with -SolCx_xc ... )                         \n", x_c);
		PetscPrintf(PETSC_COMM_WORLD,"              eta_b = %g     (change with -mumax ...    )                          \n", eta_b);
	}
	BenchmarkType = PETSC_FALSE;
	ierr 	 = PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
	if (BenchmarkType){
		eta_b = user->mumax;
		rho_a = 1;
		rho_b = 2;
		eta_a = 1;

		if (user->InputParamFile){
			/* If we use an input file take viscosity values from that file*/
			eta_a = user->PhaseProperties.mu[0];
			eta_b = user->PhaseProperties.mu[1];
			rho_a = user->PhaseProperties.rho[0];
			rho_b = user->PhaseProperties.rho[1];

		}
		PetscPrintf(PETSC_COMM_WORLD,"  using:                             FallingBlock setup                                        \n \n");
		PetscPrintf(PETSC_COMM_WORLD,"   note:      strictly speaking this is not a benchmark as we lack an analytical solution        \n");
		PetscPrintf(PETSC_COMM_WORLD,"              but it is useful to test the solver, without the effect of particle interpolation \n");
		PetscPrintf(PETSC_COMM_WORLD,"              eta_block = %f  [change with -mumax ??] \n",eta_b);



		/* Set some general parameters such as output filename for this benchmark */
		sprintf(user->OutputFile, 		"AnalyticalBenchmark_FallingBlock");
	}
	ierr 	 = PetscOptionsGetBool( PETSC_NULL, "-Benchmark_VerticalDensityCollumn",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
	if (BenchmarkType){
		eta_b = user->mumax;

		if (user->InputParamFile){
			/* If we use an input file take viscosity values from that file*/
			eta_a = user->PhaseProperties.mu[0];
			eta_b = user->PhaseProperties.mu[1];
			rho_a = user->PhaseProperties.rho[0];
			rho_b = user->PhaseProperties.rho[1];

		}



		Direction = 0;		//0=x-direction; 1=y-direction; 2=z-direction

		ierr 	 = PetscOptionsGetInt( PETSC_NULL, "-Direction",   &Direction, PETSC_NULL); CHKERRQ(ierr);


		PetscPrintf(PETSC_COMM_WORLD,"  using:                             VerticalDensityCollumn setup                                        \n \n");
		PetscPrintf(PETSC_COMM_WORLD,"   note:      strictly speaking this is not a benchmark as we lack an analytical solution        \n");
		PetscPrintf(PETSC_COMM_WORLD,"              but it is useful to test the solver, without the effect of particle interpolation \n");
		PetscPrintf(PETSC_COMM_WORLD,"              eta_block = %f     [change with -mumax ??] \n",eta_b);
		PetscPrintf(PETSC_COMM_WORLD,"              with jump in direction %i  [change with -Direction ?]\n",Direction);


		/* Set some general parameters such as output filename for this benchmark */
		sprintf(user->OutputFile, 		"AnalyticalBenchmark_VerticalDensityCollumn");
	}

	BenchmarkType = PETSC_FALSE;
	ierr 	 = PetscOptionsGetBool( PETSC_NULL, "-Benchmark_ArcTanFallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
	if (BenchmarkType){

		eta_b = user->mumax;
		PetscPrintf(PETSC_COMM_WORLD,"  using:                      Arctangent FallingBlock setup                                        \n \n");
		PetscPrintf(PETSC_COMM_WORLD,"   note:      strictly speaking this is not a benchmark as we lack an analytical solution        \n");
		PetscPrintf(PETSC_COMM_WORLD,"              but it is useful to test the solver, without the effect of particle interpolation \n");
		PetscPrintf(PETSC_COMM_WORLD,"              eta_block = %f \n",eta_b);



		ArcTan_Sharpness = 200;
		ierr = PetscOptionsGetReal(PETSC_NULL ,"-ArcTan_Sharpness",	&ArcTan_Sharpness	, PETSC_NULL); CHKERRQ(ierr);


		/* Set some general parameters such as output filename for this benchmark */
		sprintf(user->OutputFile, 		"AnalyticalBenchmark_FallingBlock");
	}


	/* Add info about additional benchmarks here */



	/* Depending on whether we are dealing with FDSTAG or a FE formulation, we have to set density & viscosity differently*/
	if (element_type != DAVP_FDSTAG){	/* FE formulation */

		PetscInt		xs, ys, zs, xm, ym, zm, iel_x, iel_y, iel_z, intp;
		PetscScalar		x,y,z, rho, eta;


		eta=1; rho=1;	// initialize variables
		ierr = DMDAVecGetArray(user->DA_Materials, user->Materials, &materials_array); CHKERRQ(ierr);
		ierr = DMDAGetCorners(user->DA_Processors,&xs,&ys,&zs,&xm,&ym,&zm); CHKERRQ(ierr);

		/* Loop over elements */
		for (iel_z=zs; iel_z<zs+zm; iel_z++){
			for (iel_y=ys; iel_y<ys+ym; iel_y++){
				for (iel_x=xs; iel_x<xs+xm; iel_x++){

					/* load data */
					LaMEMSetMaterialDataMemoryFromArray( &material_data, iel_x-xs,iel_y-ys,iel_z-zs, C->ngp_vel, materials_array );

					/* Make a loop over each integration point */
					for (intp=0; intp<C->ngp_vel; intp++){
						x	=	material_data.Coord[0][intp];
						y 	= 	material_data.Coord[1][intp];
						z	=	material_data.Coord[2][intp];

						/* SolCx benchmark */
						SolCx_y_direction 	= PETSC_FALSE;
						BenchmarkType	 	= PETSC_FALSE;
						ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-SolCx_y_direction",   &SolCx_y_direction, PETSC_NULL); CHKERRQ(ierr);
						ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
						if (BenchmarkType){
							//  classical SolCx test in x-direction
							if (x<=(x_c+1e-6)){  // add epsilon to prevent roundoff errors if x_c is exactly at the middle of the element
								eta = eta_a;
							}
							else{
								eta = eta_b;
							}
							rho 	=	-sigma*sin(nz*PI*z)*cos(nx*PI*x);

							if (SolCx_y_direction){
								//  SolCx test in y-direction
								if (y< x_c){
									eta = eta_a;
								}
								else{
									eta = eta_b;
								}
								rho 	=	-sigma*sin(nz*PI*z)*cos(nx*PI*y);
							}

						}
						BenchmarkType 	= PETSC_FALSE;
						ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
						if (BenchmarkType){
							// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
							// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
							eta = eta_a;
							rho = rho_a;
							if ( x>(user->x_left  + user->W/4) && x<(user->x_left  + 3*user->W/4) &&
								 y>(user->y_front + user->L/4) && y<(user->y_front + 3*user->L/4) &&
								 z>(user->z_bot   + user->H/4) && z<(user->z_bot   + 3*user->H/4) ){
								eta = eta_b;
								rho = rho_b;
							}


						}
						BenchmarkType 	= PETSC_FALSE;
						ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_ArcTanFallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
						if (BenchmarkType){
							// Falling block benchmark arctangent
							eta = eta_a;
							rho = 1;

							eta = eta_b*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
									   *(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
									   *(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

							rho  = 2*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
									*(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
									*(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

						}

						/* Add additional benchmarks here */



						/* Set data for the current integration point */
						material_data.Viscosity[intp]  			= eta;
						material_data.Density[intp]  			= rho;

					}

				}
			}
		}

		ierr = DMDAVecRestoreArray(user->DA_Materials,  user->Materials, &materials_array); CHKERRQ(ierr);



	}
	else{
		/* FDSTAG formulation */
		PetscInt 		xs,ys,zs,xm,ym,zm,ielx,iely,ielz, i;
		PetscScalar 	***viscosity_center, ***density_center,x,y,z, eta, rho,phase, ***viscosity_xy,***viscosity_xz,***viscosity_yz;
		PetscScalar		***PhaseProportions_local[max_num_phases];
		Vec				coord, *PhaseProportions_Vec[max_num_phases];
		DMDACoor3d		***coords;
		DM				cda;


		// initialize variables.
		eta = 1;
		rho = 1;

		/* Get viscosity and coordinates @ cell centers */
		ierr = DMDAVecGetArray(user->FDSTAG.DA_CENTER,  			user->FDSTAG.Center_EffectiveViscosity, 	&viscosity_center); 	CHKERRQ(ierr); 	// Viscosity @ center
		ierr = DMDAVecGetArray(user->FDSTAG.DA_CENTER,   			user->FDSTAG.Center_Density, 				&density_center	 ); 	CHKERRQ(ierr); 	// Density @ center

		for (i=0; i<user->num_phases; i++){
			PhaseProportions_Vec[i]	= 	&(user->FDSTAG.Center_PhaseProportions[i]);
		}
		for (i=0; i<user->num_phases; i++){
			ierr = DMDAVecGetArray(user->FDSTAG.DA_CENTER,   *PhaseProportions_Vec[i], 	&PhaseProportions_local[i]	 ); CHKERRQ(ierr); // Phase proportions
		}


		ierr = DMGetCoordinateDM(user->FDSTAG.DA_CENTER,		&cda); 																CHKERRQ(ierr);	// Coordinates
		ierr = DMGetCoordinatesLocal(user->FDSTAG.DA_CENTER,	&coord); 															CHKERRQ(ierr);
		ierr = DMDAVecGetArray(cda,coord,&coords); 																					CHKERRQ(ierr);

		ierr = DMDAGetCorners(user->FDSTAG.DA_CENTER,&xs,&ys,&zs,&xm,&ym,&zm); 														CHKERRQ(ierr);
		for (ielz=zs; ielz<zs+zm; ielz++){
			for (iely=ys; iely<ys+ym; iely++){
				for (ielx=xs; ielx<xs+xm; ielx++){

					/* Extract coordinate of current point */
					x	=	coords[ielz][iely][ielx].x;
					y	=	coords[ielz][iely][ielx].y;
					z	=	coords[ielz][iely][ielx].z;

					/* SolCx benchmark */
					SolCx_y_direction 	= PETSC_FALSE;
					BenchmarkType 		= PETSC_FALSE;
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-SolCx_y_direction",   &SolCx_y_direction, PETSC_NULL); CHKERRQ(ierr);
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   	&BenchmarkType, 	PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						//  classical SolCx test in x-direction
						phase=0;
						if (x<=x_c){
							eta = eta_a;
						}
						else{
							eta = eta_b;
						}
						rho 	=	-sigma*sin(nz*PI*z)*cos(nx*PI*x);

						if (SolCx_y_direction){
							//  SolCx test in y-direction
							if (y<=x_c){
								eta = eta_a;
							}
							else{
								eta = eta_b;
							}
							rho 	=	-sigma*sin(nz*PI*z)*cos(nx*PI*y);
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = rho_a;
						phase=0;
						if ( x>(user->x_left  + user->W/4) && x<(user->x_left  + 3*user->W/4) &&
							 y>(user->y_front + user->L/4) && y<(user->y_front + 3*user->L/4) &&
							 z>(user->z_bot   + user->H/4) && z<(user->z_bot   + 3*user->H/4) ){
							eta = eta_b;
							rho = rho_b;
							phase=1;
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_VerticalDensityCollumn",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = rho_a;
						phase=0;
						if 			(Direction==0){
							if  ( x>(user->x_left  + user->W/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if 	(Direction==1){
							if ( y>(user->y_front + user->L/2)){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if 	(Direction==2){
							if ( z>(user->z_bot   + user->H/2)){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==3){
							// x and z direction
							if  ( x>(user->x_left  + user->W/4) && z<(user->z_bot   + 3*user->H/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==4){
							// x and y direction
							if  ( x>(user->x_left  + user->W/4) && y<(user->y_front + 3*user->L/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==5){
							// y and z direction
							if  ( z<(user->z_bot   + 3*user->H/4) && y<(user->y_front + 3*user->L/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}


					}

					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_ArcTanFallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark arctangent
						eta = eta_a;
						rho = 1;
						phase=0;
						eta = eta_b*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

						rho  = 2*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

					}

					viscosity_center[ielz][iely][ielx] 	= eta;
					density_center[ielz][iely][ielx] 	= rho;

					// adapt phases
					if (phase==0){
						PhaseProportions_local[0][ielz][iely][ielx]=1;
						PhaseProportions_local[1][ielz][iely][ielx]=0;
					}
					else{
						PhaseProportions_local[0][ielz][iely][ielx]=0;
						PhaseProportions_local[1][ielz][iely][ielx]=1;
					}

				}
			}
		}

		ierr = DMDAVecRestoreArray(user->FDSTAG.DA_CENTER,   user->FDSTAG.Center_Density, 				&density_center); 	CHKERRQ(ierr);
		ierr = DMDAVecRestoreArray(user->FDSTAG.DA_CENTER,   user->FDSTAG.Center_EffectiveViscosity, 	&viscosity_center); CHKERRQ(ierr);
		ierr = DMDAVecRestoreArray(cda,					coord,									&coords); 			CHKERRQ(ierr);
		for (i=0; i<user->num_phases; i++ ){
			ierr = DMDAVecRestoreArray(user->FDSTAG.DA_CENTER,   	*PhaseProportions_Vec[i], 	&PhaseProportions_local[i]	 ); CHKERRQ(ierr);
		}

		//ierr = VecDestroy(coord); CHKERRQ(ierr);
		//ierr = DMDestroy( cda ); 			CHKERRQ(ierr);


		/* Get viscosity and coordinates @ XY points */
		ierr = DMDAVecGetArray(user->FDSTAG.DA_XY_POINTS,  		user->FDSTAG.XYPoints_EffectiveViscosity, 	&viscosity_xy	); 	CHKERRQ(ierr); 	// Viscosity
		ierr = DMGetCoordinateDM(user->FDSTAG.DA_XY_POINTS,			&cda); 														CHKERRQ(ierr);	// Coordinates
		ierr = DMGetCoordinatesLocal(user->FDSTAG.DA_XY_POINTS,	&coord); 													CHKERRQ(ierr);
		ierr = DMDAVecGetArray(cda,coord,&coords); 																				CHKERRQ(ierr);

		for (i=0; i<user->num_phases; i++){
			PhaseProportions_Vec[i]	= 	&(user->FDSTAG.XYPoints_PhaseProportions[i]);
		}
		for (i=0; i<user->num_phases; i++){
			ierr = DMDAVecGetArray(user->FDSTAG.DA_XY_POINTS,   *PhaseProportions_Vec[i], 	&PhaseProportions_local[i]	 ); CHKERRQ(ierr); // Phase proportions
		}


		ierr = DMDAGetCorners(user->FDSTAG.DA_XY_POINTS,&xs,&ys,&zs,&xm,&ym,&zm); CHKERRQ(ierr);
		for (ielz=zs; ielz<zs+zm; ielz++){
			for (iely=ys; iely<ys+ym; iely++){
				for (ielx=xs; ielx<xs+xm; ielx++){

					/* Extract coordinate of current point */
					x	=	coords[ielz][iely][ielx].x;
					y	=	coords[ielz][iely][ielx].y;
					z	=	coords[ielz][iely][ielx].z;

					/* SolCx benchmark */
					BenchmarkType 		= PETSC_FALSE;
					SolCx_y_direction 	= PETSC_FALSE;
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-SolCx_y_direction",   &SolCx_y_direction, PETSC_NULL); CHKERRQ(ierr);
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						phase = 0;
						if (x<=x_c){
							eta = eta_a;
						}
						else{
							eta = eta_b;
						}

						if (SolCx_y_direction){
							//  SolCx test in y-direction
							if (y<=x_c){
								eta = eta_a;
							}
							else{
								eta = eta_b;
							}
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = rho_a;
						phase = 0;
						if ( x>(user->x_left  + user->W/4) && x<(user->x_left  + 3*user->W/4) &&
							 y>(user->y_front + user->L/4) && y<(user->y_front + 3*user->L/4) &&
							 z>(user->z_bot   + user->H/4) && z<(user->z_bot   + 3*user->H/4) ){
							eta = eta_b;
							rho = rho_b;
							phase = 1;
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_VerticalDensityCollumn",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = rho_a;
						phase=0;
						if 			(Direction==0){
							if  ( x>(user->x_left  + user->W/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if 	(Direction==1){
							if ( y>(user->y_front + user->L/2)){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if 	(Direction==2){
							if ( z>(user->z_bot   + user->H/2)){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==3){
							// x and z direction
							if  ( x>(user->x_left  + user->W/4) && z<(user->z_bot   + 3*user->H/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==4){
							// x and y direction
							if  ( x>(user->x_left  + user->W/4) && y<(user->y_front + 3*user->L/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==5){
							// y and z direction
							if  ( z<(user->z_bot   + 3*user->H/4) && y<(user->y_front + 3*user->L/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_ArcTanFallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark arctangent
						eta = eta_a;
						rho = 1;
						phase = 0;
						eta = eta_b*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

						rho  = 2*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

					}

					viscosity_xy[ielz][iely][ielx] 	= eta;


					// adapt phases
					if (phase==0){
						PhaseProportions_local[0][ielz][iely][ielx]=1;
						PhaseProportions_local[1][ielz][iely][ielx]=0;
					}
					else{
						PhaseProportions_local[0][ielz][iely][ielx]=0;
						PhaseProportions_local[1][ielz][iely][ielx]=1;
					}

				}
			}
		}
		ierr = DMDAVecRestoreArray(user->FDSTAG.DA_XY_POINTS,   user->FDSTAG.XYPoints_EffectiveViscosity, 		&viscosity_xy); CHKERRQ(ierr);
		ierr = DMDAVecRestoreArray(cda,					coord,								&coords); 			CHKERRQ(ierr);
//		ierr = VecDestroy(coord); CHKERRQ(ierr);
//		ierr = DMDestroy( cda ); 			CHKERRQ(ierr);

		for (i=0; i<user->num_phases; i++ ){
			ierr = DMDAVecRestoreArray(user->FDSTAG.DA_XY_POINTS,   	*PhaseProportions_Vec[i], 	&PhaseProportions_local[i]	 ); CHKERRQ(ierr);
		}

		/* Get viscosity and coordinates @ XZ points */
		ierr = DMDAVecGetArray(user->FDSTAG.DA_XZ_POINTS,  			user->FDSTAG.XZPoints_EffectiveViscosity, 	&viscosity_xz	); 	CHKERRQ(ierr); 	// Viscosity
		ierr = DMGetCoordinateDM(user->FDSTAG.DA_XZ_POINTS,			&cda); 										CHKERRQ(ierr);						// Coordinates
		ierr = DMGetCoordinatesLocal(user->FDSTAG.DA_XZ_POINTS,	&coord); 									CHKERRQ(ierr);
		ierr = DMDAVecGetArray(cda,coord,&coords); 																CHKERRQ(ierr);

		for (i=0; i<user->num_phases; i++){
			PhaseProportions_Vec[i]	= 	&(user->FDSTAG.XZPoints_PhaseProportions[i]);
		}
		for (i=0; i<user->num_phases; i++){
			ierr = DMDAVecGetArray(user->FDSTAG.DA_XZ_POINTS,   *PhaseProportions_Vec[i], 	&PhaseProportions_local[i]	 ); CHKERRQ(ierr); // Phase proportions
		}


		ierr = DMDAGetCorners(user->FDSTAG.DA_XZ_POINTS,&xs,&ys,&zs,&xm,&ym,&zm); CHKERRQ(ierr);
		for (ielz=zs; ielz<zs+zm; ielz++){
			for (iely=ys; iely<ys+ym; iely++){
				for (ielx=xs; ielx<xs+xm; ielx++){

					/* Extract coordinate of current point */
					x	=	coords[ielz][iely][ielx].x;
					y	=	coords[ielz][iely][ielx].y;
					z	=	coords[ielz][iely][ielx].z;

					/* SolCx benchmark */
					BenchmarkType 		= PETSC_FALSE;
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						phase=0;
						if (x<=x_c){
							eta = eta_a;
						}
						else{
							eta = eta_b;
						}

						if (SolCx_y_direction){
							//  SolCx test in y-direction
							if (y<=x_c){
								eta = eta_a;
							}
							else{
								eta = eta_b;
							}
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = rho_a;
						phase=0;
						if ( x>(user->x_left  + user->W/4) && x<(user->x_left  + 3*user->W/4) &&
							 y>(user->y_front + user->L/4) && y<(user->y_front + 3*user->L/4) &&
							 z>(user->z_bot   + user->H/4) && z<(user->z_bot   + 3*user->H/4) ){
							eta = eta_b;
							rho = rho_b;
							phase = 1;
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_VerticalDensityCollumn",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = rho_a;
						phase=0;
						if 			(Direction==0){
							if  ( x>(user->x_left  + user->W/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if 	(Direction==1){
							if ( y>(user->y_front + user->L/2)){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if 	(Direction==2){
							if ( z>(user->z_bot   + user->H/2)){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==3){
							// x and z direction
							if  ( x>(user->x_left  + user->W/4) && z<(user->z_bot   + 3*user->H/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==4){
							// x and y direction
							if  ( x>(user->x_left  + user->W/4) && y<(user->y_front + 3*user->L/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==5){
							// y and z direction
							if  ( z<(user->z_bot   + 3*user->H/4) && y<(user->y_front + 3*user->L/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}

					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_ArcTanFallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark arctangent
						eta = eta_a;
						rho = 1;
						phase = 0;

						eta = eta_b*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

						rho  = 2*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

					}

					viscosity_xz[ielz][iely][ielx] 	= eta;

					// adapt phases
					if (phase==0){
						PhaseProportions_local[0][ielz][iely][ielx]=1;
						PhaseProportions_local[1][ielz][iely][ielx]=0;
					}
					else{
						PhaseProportions_local[0][ielz][iely][ielx]=0;
						PhaseProportions_local[1][ielz][iely][ielx]=1;
					}

				}
			}
		}
		ierr = DMDAVecRestoreArray(user->FDSTAG.DA_XZ_POINTS,   user->FDSTAG.XZPoints_EffectiveViscosity, 		&viscosity_xz); CHKERRQ(ierr);
		ierr = DMDAVecRestoreArray(cda,					coord,								&coords); 			CHKERRQ(ierr);
//		ierr = VecDestroy(coord); 			CHKERRQ(ierr);
//		ierr = DMDestroy( cda ); 			CHKERRQ(ierr);
		for (i=0; i<user->num_phases; i++ ){
			ierr = DMDAVecRestoreArray(user->FDSTAG.DA_XZ_POINTS,   	*PhaseProportions_Vec[i], 	&PhaseProportions_local[i]	 ); CHKERRQ(ierr);
		}

	/* Get viscosity and coordinates @ YZ points */
		ierr = DMDAVecGetArray(user->FDSTAG.DA_YZ_POINTS,  			user->FDSTAG.YZPoints_EffectiveViscosity, 	&viscosity_yz	); 	CHKERRQ(ierr); // Viscosity
		ierr = DMGetCoordinateDM(user->FDSTAG.DA_YZ_POINTS,			&cda); 											CHKERRQ(ierr);	// Coordinates
		ierr = DMGetCoordinatesLocal(user->FDSTAG.DA_YZ_POINTS,	&coord); 										CHKERRQ(ierr);
		ierr = DMDAVecGetArray(cda,coord,&coords); 																CHKERRQ(ierr);
		for (i=0; i<user->num_phases; i++){
			PhaseProportions_Vec[i]	= 	&(user->FDSTAG.YZPoints_PhaseProportions[i]);
		}
		for (i=0; i<user->num_phases; i++){
			ierr = DMDAVecGetArray(user->FDSTAG.DA_YZ_POINTS,   *PhaseProportions_Vec[i], 	&PhaseProportions_local[i]	 ); CHKERRQ(ierr); // Phase proportions
		}


		ierr = DMDAGetCorners(user->FDSTAG.DA_YZ_POINTS,&xs,&ys,&zs,&xm,&ym,&zm); CHKERRQ(ierr);
		for (ielz=zs; ielz<zs+zm; ielz++){
			for (iely=ys; iely<ys+ym; iely++){
				for (ielx=xs; ielx<xs+xm; ielx++){

					/* Extract coordinate of current point */
					x	=	coords[ielz][iely][ielx].x;
					y	=	coords[ielz][iely][ielx].y;
					z	=	coords[ielz][iely][ielx].z;

					/* SolCx benchmark */
					BenchmarkType 		= PETSC_FALSE;
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						phase = 0;
						if (x<=x_c){
							eta = eta_a;
						}
						else{
							eta = eta_b;
						}

						if (SolCx_y_direction){
							//  SolCx test in y-direction
							if (y<=x_c){
								eta = eta_a;
							}
							else{
								eta = eta_b;
							}
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = rho_a;
						phase = 0;
						if ( x>(user->x_left  + user->W/4) && x<(user->x_left  + 3*user->W/4) &&
							 y>(user->y_front + user->L/4) && y<(user->y_front + 3*user->L/4) &&
							 z>(user->z_bot   + user->H/4) && z<(user->z_bot   + 3*user->H/4) ){
							eta = eta_b;
							rho = rho_b;
							phase=1;
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_VerticalDensityCollumn",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = rho_a;
						phase=0;
						if 			(Direction==0){
							if  ( x>(user->x_left  + user->W/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if 	(Direction==1){
							if ( y>(user->y_front + user->L/2)){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if 	(Direction==2){
							if ( z>(user->z_bot   + user->H/2)){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==3){
							// x and z direction
							if  ( x>(user->x_left  + user->W/4) && z<(user->z_bot   + 3*user->H/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==4){
							// x and y direction
							if  ( x>(user->x_left  + user->W/4) && y<(user->y_front + 3*user->L/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
						else if (Direction==5){
							// y and z direction
							if  ( z<(user->z_bot   + 3*user->H/4) && y<(user->y_front + 3*user->L/4) ){
								eta = eta_b;
								rho = rho_b;
								phase=1;
							}
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_ArcTanFallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark arctangent
						eta = eta_a;
						rho = 1;
						phase=0;
						eta = eta_b*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

						rho  = 2*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

					}

					viscosity_yz[ielz][iely][ielx] 	= eta;

					// adapt phases
					if (phase==0){
						PhaseProportions_local[0][ielz][iely][ielx]=1;
						PhaseProportions_local[1][ielz][iely][ielx]=0;
					}
					else{
						PhaseProportions_local[0][ielz][iely][ielx]=0;
						PhaseProportions_local[1][ielz][iely][ielx]=1;
					}

				}
			}
		}
		ierr = DMDAVecRestoreArray(user->FDSTAG.DA_YZ_POINTS,   user->FDSTAG.YZPoints_EffectiveViscosity, 		&viscosity_yz); CHKERRQ(ierr);
		ierr = DMDAVecRestoreArray(cda,					coord,								&coords); 			CHKERRQ(ierr);
//		ierr = VecDestroy(coord); 			CHKERRQ(ierr);
//		ierr = DMDestroy( cda ); 			CHKERRQ(ierr);

		for (i=0; i<user->num_phases; i++ ){
			ierr = DMDAVecRestoreArray(user->FDSTAG.DA_YZ_POINTS,   	*PhaseProportions_Vec[i], 	&PhaseProportions_local[i]	 ); CHKERRQ(ierr);
		}

#if 0
		/* Get density and coordinates @ center  points */
		ierr = DMDAVecGetArray(user->FDSTAG.DA_CENTER,  			user->FDSTAG.Center_Density, 	&density_vz	); 	CHKERRQ(ierr); // density
		ierr = DMGetCoordinateDM(user->FDSTAG.DA_CENTER,		&cda); 											CHKERRQ(ierr);	// Coordinates
		ierr = DMGetCoordinatesLocal(user->FDSTAG.DA_CENTER,	&coord); 										CHKERRQ(ierr);
		ierr = DMDAVecGetArray(cda,coord,&coords); 																CHKERRQ(ierr);


		ierr = DMDAGetCorners(user->FDSTAG.DA_CENTER,&xs,&ys,&zs,&xm,&ym,&zm); CHKERRQ(ierr);
		for (ielz=zs; ielz<zs+zm; ielz++){
			for (iely=ys; iely<ys+ym; iely++){
				for (ielx=xs; ielx<xs+xm; ielx++){

					/* Extract coordinate of current point */
					x	=	coords[ielz][iely][ielx].x;
					y	=	coords[ielz][iely][ielx].y;
					z	=	coords[ielz][iely][ielx].z;

					/* SolCx benchmark */
					BenchmarkType 		= PETSC_FALSE;
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						rho 	=	-sigma*sin(nz*PI*z)*cos(nx*PI*x);


						if (SolCx_y_direction){
							//  SolCx test in y-direction
							rho 	=	-sigma*sin(nz*PI*z)*cos(nx*PI*y);
						}

					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = 1;
						if ( x>(user->x_left  + user->W/4) && x<(user->x_left  + 3*user->W/4) &&
							 y>(user->y_front + user->L/4) && y<(user->y_front + 3*user->L/4) &&
							 z>(user->z_bot   + user->H/4) && z<(user->z_bot   + 3*user->H/4) ){
							eta = eta_b;
							rho = 2;
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_VerticalDensityCollumn",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark - this is strictly speaking not a benchmark as we don't have an analytical solution for it, but
						// by adding it here, we can test the effect of setting viscosity directly, rather then interpolating viscosity from particles
						eta = eta_a;
						rho = 1;
						phase=0;
						if ( x>(user->x_left  + user->W/2)){
							eta = eta_b;
							rho = 2;
							phase=1;
						}
					}
					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_ArcTanFallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block benchmark arctangent
						eta = eta_a;
						rho = 1;

						eta = eta_b*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								   *(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

						rho  = 2*(atan(ArcTan_Sharpness*(x-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(x-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(y-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(y-0.6))/3.2+0.5)
								*(atan(ArcTan_Sharpness*(z-0.4))/3.2+0.5)*(atan(-ArcTan_Sharpness*(z-0.6))/3.2+0.5) + 1;

					}

					density_vz[ielz][iely][ielx] 	= rho;

				}
			}
		}
		ierr = DMDAVecRestoreArray(user->FDSTAG.DA_CENTER,   user->FDSTAG.Center_Density, 		&density_vz); 	CHKERRQ(ierr);
		ierr = DMDAVecRestoreArray(cda,					coord,									&coords); 		CHKERRQ(ierr);
//		ierr = VecDestroy(coord); 			CHKERRQ(ierr);
//		ierr = DMDestroy( cda ); 			CHKERRQ(ierr);
#endif

	}



	/* Finish printing output to the screen */
	PetscPrintf(PETSC_COMM_WORLD,"\n --------------------------------------------------------------------------------  \n \n \n");


	PetscFunctionReturn(0);
}
/*==============================================================================================================================*/



/*
	Compares numerical results versus a hardcoded analytical solution
*/
#undef __FUNCT__
#define __FUNCT__ "LaMEM_CompareNumerics_vs_AnalyticalSolution"
PetscErrorCode LaMEM_CompareNumerics_vs_AnalyticalSolution( UserContext *user, LaMEMVelPressureDA C,
		Vec sol, Vec Pressure)
{
	PetscBool 			BenchmarkType, SolCx_y_direction, Benchmark_PlotErrors_VP;
	PetscErrorCode 		ierr;
	DAVPElementType 	element_type;
	PetscScalar 		Vx_error, Vy_error, Vz_error, V_error, P_error, E_error, S_error, dx,dy,dz, P_error2;
	/* Define various parameters for this specific benchmark */
	PetscScalar 			nz=1.0, eta_a=1.0, eta_b, x_c;

	element_type = C->type;


	PetscPrintf(PETSC_COMM_WORLD,"\n\n --------------------------------------------------------------------------------  \n\n");
	PetscPrintf(PETSC_COMM_WORLD,"  LaMEM STOKES BENCHMARK RESULTS:                                                 \n");




	/* Default parameters */
	x_c   = user->x_left + user->W/2;
	ierr = PetscOptionsGetReal(PETSC_NULL ,"-SolCx_xc",	&x_c	, PETSC_NULL); CHKERRQ(ierr);
	eta_b = user->mumax;

	/* Replace the velocities with the deviation of velocity versus the analytical solution*/
	Benchmark_PlotErrors_VP = PETSC_FALSE;
	ierr 	 = PetscOptionsGetBool( PETSC_NULL, "-Benchmark_PlotErrors_VP",   &Benchmark_PlotErrors_VP, PETSC_NULL); CHKERRQ(ierr);


	/* Depending on whether we are dealing with FDSTAG or a FE formulation, we have to set density & viscosity differently*/
	if (element_type != DAVP_FDSTAG){	/* FE formulation */

		PetscInt				xs, ys, zs, xm, ym, zm, ix, iy, iz, intp;
		PetscInt 				iel_x,iel_y,iel_z;
		PetscScalar				x,y,z, Vx_num, Vy_num, Vz_num, P_num;
		PetscScalar				Vx_err_int, Vy_err_int, Vz_err_int, P_err_int, E_err_int,  S_err_int, P_err2_int;
		PetscScalar				Exx_num, Eyy_num, Ezz_num, Exz_num, Eyz_num, Exy_num;
		PetscScalar				Sxx_num, Syy_num, Szz_num, Sxz_num, Syz_num, Sxy_num;
		PetscScalar				Volume_local, Volume;
		PetscScalar				pos[2], vel[2], pressure, total_stress[3], strain_rate[3];
		Vec						coord, local_Vel;
		DMDACoor3d				***coords;
		Field 					***velocity;
		PetscScalar 			***materials_array;
		MaterialsElementDynamic material_data;

		ierr = DMDAGetCorners (user->DA_Vel, &xs, &ys, &zs, &xm, &ym, &zm); CHKERRQ(ierr);

		/* Get the coordinates from the DMDA */
		ierr = DMGetCoordinates(user->DA_Vel,&coord); CHKERRQ(ierr);
		ierr = DMDAVecGetArray(user->DA_Vel,coord,&coords); CHKERRQ(ierr);

		/* Get velocity from the DMDA */
		ierr = DMGetLocalVector(user->DA_Vel,&local_Vel); CHKERRQ(ierr);
		ierr = DMGlobalToLocalBegin(user->DA_Vel, sol, INSERT_VALUES, local_Vel); CHKERRQ(ierr);
		ierr = DMGlobalToLocalEnd(user->DA_Vel,   sol, INSERT_VALUES, local_Vel); CHKERRQ(ierr);
		ierr = DMDAVecGetArray(user->DA_Vel, local_Vel,	&velocity); CHKERRQ(ierr);

		/* Spacing for element (assumed to be constant in domain!) */
		dx 	= coords[zs+1][ys+1][xs+1].x-coords[zs][ys][xs].x;
		dy 	= coords[zs+1][ys+1][xs+1].y-coords[zs][ys][xs].y;
		dz 	= coords[zs+1][ys+1][xs+1].z-coords[zs][ys][xs].z;

		/* Loop over all nodes and evaluate error in velocity */
		Volume_local = 0.0;Vx_err_int=0.0; Vy_err_int=0.0; Vz_err_int=0.0;
		for (iz=zs; iz<zs+zm; iz++){
			for (iy=ys; iy<ys+ym; iy++){
				for (ix=xs; ix<xs+xm; ix++){

					/* retrieve numerical parameters for the current nodal point */
					x 		= coords[iz][iy][ix].x;
					y 		= coords[iz][iy][ix].y;
					z 		= coords[iz][iy][ix].z;
					Vx_num 	= velocity[iz][iy][ix].Vx;
					Vy_num 	= velocity[iz][iy][ix].Vy;
					Vz_num 	= velocity[iz][iy][ix].Vz;


					/* SolCx benchmark
					 *
					 * density in this benchmark is prescribed as:  rho = -sigma*sin(nz*PI*z)*cos(nx*PI*x)
					 * viscosity varies in x-direction and is eta_a for x<x_c and eta_b for x>x_c
					 *
					 */
					SolCx_y_direction = PETSC_FALSE;
					BenchmarkType = PETSC_FALSE;
					ierr 	 = PetscOptionsGetBool( PETSC_NULL, "-SolCx_y_direction",   &SolCx_y_direction, PETSC_NULL); CHKERRQ(ierr);
					ierr 	 = PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						/* Compute analytical solution @ velocity nodes*/
						pos[0] = x; pos[1]=z;		// default is in x-z direction

						if (SolCx_y_direction){
							pos[0] = y; pos[1]=z;		// SolCx in y-direction
						}

						solCx( pos, eta_a, eta_b, x_c, (PetscInt)nz, vel, &pressure, total_stress, strain_rate );
					}

					/* Evaluate pointwise error */
					Vx_error = Vx_num-vel[0]; 		Vx_error = PetscAbs(Vx_error);
					Vy_error = Vy_num-0; 			Vy_error = PetscAbs(Vy_error);
					Vz_error = Vz_num-vel[1]; 		Vz_error = PetscAbs(Vz_error);

					if (SolCx_y_direction){
						// SolCx in y-direction
						Vx_error = Vx_num-0; 		Vx_error = PetscAbs(Vx_error);
						Vy_error = Vy_num-vel[0]; 	Vy_error = PetscAbs(Vy_error);
						Vz_error = Vz_num-vel[1]; 	Vz_error = PetscAbs(Vz_error);
					}

					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block test. Since we have no analytical solution, the error is zero.
						Vx_error = 0;
						Vy_error = 0;
						Vz_error = 0;
					}

					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_VerticalDensityCollumn",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Vertical collumn test. Since we have no analytical solution, the error is zero.
						Vx_error = 0;
						Vy_error = 0;
						Vz_error = 0;
					}

					/* Integrate values to compute L1 error */
					Volume_local = Volume_local + dx*dy*dz;
					Vx_err_int 	 = Vx_err_int 	+ Vx_error*dx*dy*dz;
					Vy_err_int 	 = Vy_err_int 	+ Vy_error*dx*dy*dz;
					Vz_err_int 	 = Vz_err_int 	+ Vz_error*dx*dy*dz;

					if (Benchmark_PlotErrors_VP){
						// for debugging
						velocity[iz][iy][ix].Vx = Vx_error;
						velocity[iz][iy][ix].Vy = Vy_error;
						velocity[iz][iy][ix].Vz = Vz_error;
					}

					/* for debugging: print data to screen */
					//PetscPrintf(PETSC_COMM_SELF,"coord=[%g,%g,%g] V_num=[%g,%g,%g] V_an=[%g,%g] \n", ix, iy, iz, x,y,z, Vx_num, Vy_num, Vz_num, vel[0], vel[1]);


				}
			}
		}

		/* Restore coordinates and velocity */
		ierr = DMDAVecRestoreArray(user->DA_Vel,coord,&coords); CHKERRQ(ierr);

		ierr = DMDAVecRestoreArray(user->DA_Vel, local_Vel,	&velocity); CHKERRQ(ierr);
		ierr = DMRestoreLocalVector(user->DA_Vel,&local_Vel); CHKERRQ(ierr);
		ierr = DMLocalToGlobalBegin(user->DA_Vel,local_Vel,INSERT_VALUES,sol);CHKERRQ(ierr);
		ierr = DMLocalToGlobalEnd  (user->DA_Vel,local_Vel,INSERT_VALUES,sol);CHKERRQ(ierr);



		/* collect data from all procs */
		ierr = MPI_Allreduce(&Volume_local,	&Volume,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&Vx_err_int,		&Vx_error,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&Vy_err_int,		&Vy_error,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&Vz_err_int,		&Vz_error,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		V_error = Vx_error/Volume + Vy_error/Volume + Vz_error/Volume;

		/*
		 * Loop over all elements and collect data on pressure, strain rate and stress which are stored on integration points.
		 * This is also test on whether these values are computed correctly
		 */
		ierr = DMDAVecGetArray(user->DA_Materials, user->Materials, &materials_array); CHKERRQ(ierr);
		ierr = DMDAGetCorners(user->DA_Processors,&xs,&ys,&zs,&xm,&ym,&zm); CHKERRQ(ierr);
		Volume_local = 0.0; P_err_int = 0.0; E_err_int=0.0; S_err_int=0.0; P_err2_int = 0.0;
		for (iel_z=zs; iel_z<zs+zm; iel_z++){
			for (iel_y=ys; iel_y<ys+ym; iel_y++){
				for (iel_x=xs; iel_x<xs+xm; iel_x++){
					ierr = LaMEMSetMaterialDataMemoryFromArray( &material_data, iel_x-xs,iel_y-ys,iel_z-zs, C->ngp_vel, materials_array ); CHKERRQ(ierr);


					/* Make a loop over each integration point
					 *
					 * Note: in order to compute the L1 error, I average the values
					 * */
					for (intp=0; intp<C->ngp_vel; intp++){

						/* Extract data */
						x		=	material_data.Coord[0][intp];
						y 		= 	material_data.Coord[1][intp];
						z		=	material_data.Coord[2][intp];
						P_num 	=	material_data.Pressure[intp];
						Exx_num =	material_data.DevStrainrate[0][intp];
						Eyy_num =	material_data.DevStrainrate[1][intp];
						Ezz_num =	material_data.DevStrainrate[2][intp];
						Exz_num =	material_data.DevStrainrate[3][intp];
						Exy_num =	material_data.DevStrainrate[4][intp];
						Eyz_num =	material_data.DevStrainrate[5][intp];

						Sxx_num =	material_data.DevStress[0][intp] - P_num;
						Syy_num =	material_data.DevStress[1][intp] - P_num;
						Szz_num =	material_data.DevStress[2][intp] - P_num;
						Sxz_num =	material_data.DevStress[3][intp];
						Sxy_num =	material_data.DevStress[4][intp];
						Syz_num =	material_data.DevStress[5][intp];


						/* SolCx benchmark
						 *
						 * density in this benchmark is prescribed as:  rho = -sigma*sin(nz*PI*z)*cos(nx*PI*x)
						 * viscosity varies in x-direction and is eta_a for x<x_c and eta_b for x>x_c
						 *
						 */
						SolCx_y_direction 	= PETSC_FALSE;
						BenchmarkType 		= PETSC_FALSE;
						ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
						ierr 		 		= PetscOptionsGetBool( PETSC_NULL, "-SolCx_y_direction",   &SolCx_y_direction, PETSC_NULL); CHKERRQ(ierr);
						if (BenchmarkType){
							/* Compute analytical solution @ velocity nodes*/
							pos[0] = x; pos[1]=z;		// default is in x-z direction
							if (SolCx_y_direction){
								pos[0] = y; pos[1]=z;		// SolCx in y-direction
							}
							solCx( pos, eta_a, eta_b, x_c, (PetscInt)nz, vel, &pressure, total_stress, strain_rate );
						}

						/* Evaluate pointwise error */
						P_error  =  PetscAbs(P_num-pressure);	// pressure

						P_error2 =	(P_num-pressure)*(P_num-pressure);


						E_error = PetscAbs(Exx_num-strain_rate[0]) 	+  PetscAbs(Ezz_num-strain_rate[1]) 	+  0*PetscAbs(Exz_num-strain_rate[2]) +
								  PetscAbs(Eyy_num-0) 				+  PetscAbs(Eyz_num-0) 					+  PetscAbs(Exy_num-0) ;																// Strainrate

						S_error = PetscAbs(Sxx_num-total_stress[0]) +  PetscAbs(Szz_num-total_stress[1]) 	+  0*PetscAbs(Sxz_num-total_stress[2]) +
								  PetscAbs(Syy_num-0) 				+  PetscAbs(Syz_num-0) 					+  PetscAbs(Sxy_num-0) ;																// total stress  [not sure whether this is correct in SolCx, so I'll leave it out]

						if (SolCx_y_direction){
							// SolCx in y-direction
							E_error = PetscAbs(Eyy_num-strain_rate[0]) 	+  PetscAbs(Ezz_num-strain_rate[1]) 	+  0*PetscAbs(Exz_num-strain_rate[2]) +
									  PetscAbs(Exx_num-0) 				+  PetscAbs(Eyz_num-0) 					+  PetscAbs(Exy_num-0) ;																// Strainrate

							S_error = PetscAbs(Syy_num-total_stress[0]) +  PetscAbs(Szz_num-total_stress[1]) 	+  0*PetscAbs(Sxz_num-total_stress[2]) +
									  PetscAbs(Sxx_num-0) 				+  PetscAbs(Syz_num-0) 					+  PetscAbs(Sxy_num-0) ;																// total stress  [not sure whether this is correct in SolCx, so I'll leave it out]


						}
						BenchmarkType 		= PETSC_FALSE;
						ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
						if (BenchmarkType){
							// Falling block test. Since we have no analytical solution, the error is zero.
							P_error = 0;
							P_error2= 0;
							S_error = 0;
							E_error = 0;
						}



						/* Integrate values to compute L1 error
						 * 	Note: strictly speaking we do not compute a volume here but ngp_vel times the real volume.
						 * As we divide later by Volume_local, this yields the same result.
						 */
						P_err_int 	 = P_err_int 	+ P_error *material_data.ElementVolume[intp];
						P_err2_int 	 = P_err2_int 	+ P_error2*material_data.ElementVolume[intp];
						E_err_int 	 = E_err_int 	+ E_error *material_data.ElementVolume[intp];
						S_err_int 	 = S_err_int 	+ S_error *material_data.ElementVolume[intp];

						Volume_local = Volume_local + material_data.ElementVolume[intp];

						/* for debugging: print data to screen */
						//PetscPrintf(PETSC_COMM_WORLD,"coord=[%g,%g,%g] P_num=[%g] P_an=[%g] E_num=[%g,%g,%g,%g,%g,%g] E_an=[%g,%g,%g] E_error=%g \n", x,y,z, P_num, pressure, Exx_num, Eyy_num, Ezz_num, Exy_num, Exz_num, Eyz_num, strain_rate[0], strain_rate[1], strain_rate[2], E_error);
						//PetscPrintf(PETSC_COMM_WORLD,"coord=[%g,%g,%g]  S_num=[%g,%g,%g,%g,%g,%g] S_an=[%g,%g,%g]  S_error=%g \n", x,y,z, Sxx_num, Syy_num, Szz_num, Sxy_num, Sxz_num, Syz_num, total_stress[0], total_stress[1], total_stress[2], S_error);

					}


				}
			}
		}

		/* restore array */
		ierr = DMDAVecRestoreArray(user->DA_Materials,  user->Materials, &materials_array); CHKERRQ(ierr);

		/* collect data from all procs */
		ierr = MPI_Allreduce(&Volume_local,	&Volume,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&P_err_int,		&P_error,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&P_err2_int,		&P_error2,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&E_err_int,		&E_error,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&S_err_int,		&S_error,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);

		/* Volume average */
		P_error = P_error/Volume;
		E_error = E_error/Volume;
		S_error = S_error/Volume;

		P_error2 = P_error2/Volume;  P_error2 = PetscSqr(P_error2); //L2 error of P


	}
	else{		/* FDSTAG formulation */


		PetscInt				xs, ys, zs, xm, ym, zm, ix, iy, iz, intp, nnode_x, nnode_y, nnode_z, InterpolatedVelocity	;
		PetscScalar				x,y,z, Vx_num, Vy_num, Vz_num, P_num, Vx_an, Vz_an;

		PetscScalar				Vx_err_int, Vy_err_int, Vz_err_int, P_err_int, E_err_int, P_err2_int;
		PetscScalar				Exx_num, Eyy_num, Ezz_num, Exz_num, Eyz_num, Exy_num;
		PetscScalar				Sxx_num, Syy_num, Szz_num, Sxz_num, Syz_num, Sxy_num;
		PetscScalar				Volume_local, Volume, ***P, P_an;
		DM						cda;
		Vec						coord, local_P;
		DMDACoor3d				***coords;
		Field 					***velocity;
		PetscScalar				pos[2], vel[2], pressure, total_stress[3], strain_rate[3];
		MaterialsElementDynamic material_data;
		PetscScalar 			***materials_array;

		InterpolatedVelocity = 1;		// original FDSTAG stencil; 1- velocity is interpolated from center (FDSTAG) nodes to corner nodes (DMDA nodes)

		/* Get the coordinates from the DMDA */
		ierr = DMGetCoordinates(user->DA_Vel,&coord); CHKERRQ(ierr);
		ierr = DMDAVecGetArray(user->DA_Vel,coord,&coords); CHKERRQ(ierr);

		ierr = DMDAVecGetArray(user->DA_Vel, sol, &velocity); 	CHKERRQ(ierr);
		ierr = DMDAGetCorners (user->DA_Vel, &xs, &ys, &zs, &xm, &ym, &zm); CHKERRQ(ierr);
		// # of nodes in all directions
		ierr = DMDAGetInfo(user->DA_Vel, 0,&nnode_x,  &nnode_y,	&nnode_z,	0,0,0,0,0,0,0,0,0);		CHKERRQ(ierr);  

		// Spacing for element (assumed to be constant in domain!)
		dx 	= coords[zs+1][ys+1][xs+1].x-coords[zs][ys][xs].x;
		dy 	= coords[zs+1][ys+1][xs+1].y-coords[zs][ys][xs].y;
		dz 	= coords[zs+1][ys+1][xs+1].z-coords[zs][ys][xs].z;


		/* Loop over all nodes and evaluate error in velocity */
		Volume_local = 0;Vx_err_int=0; Vy_err_int=0; Vz_err_int=0;
		for (iz=zs; iz<zs+zm; iz++){
			for (iy=ys; iy<ys+ym; iy++){
				for (ix=xs; ix<xs+xm; ix++){

					/* retrieve numerical parameters for the current nodal point */
					x 		= coords[iz][iy][ix].x;
					y 		= coords[iz][iy][ix].y;
					z 		= coords[iz][iy][ix].z;
					Vx_num 	= velocity[iz][iy][ix].Vx;
					Vy_num 	= velocity[iz][iy][ix].Vy;
					Vz_num 	= velocity[iz][iy][ix].Vz;

					/* One important thing to know is that because we use a staggered formulation,
					 * in combination with the DM, velocities are actually not located at (x,y,z) but rather shifted.
					 * Vx, for example, is located at (x,y+dy/2, z+dz/2) etc.
					 *
					 * This has to be taken into account while computing the analytical solution!
					 */

					/* SolCx benchmark
					 */
					BenchmarkType 		= PETSC_FALSE;
					SolCx_y_direction 	= PETSC_FALSE;
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-SolCx_y_direction",   &SolCx_y_direction, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){


						if (InterpolatedVelocity==0){
							// Vx velocity
							pos[0] = x; pos[1]=z+dz/2;		// default is in x-z direction
							if (SolCx_y_direction){
								// SolCx in y-direction
								pos[0] = y; pos[1]=z+dz/2;
							}
							solCx( pos, eta_a, eta_b, x_c, (PetscInt)nz, vel, &pressure, total_stress, strain_rate );
							Vx_an = vel[0];


							// Vz velocity
							pos[0] = x+dx/2; pos[1]=z;		// default is in x-z direction
							if (SolCx_y_direction){
								// SolCx in y-direction
								pos[0] = y+dy/2; pos[1]=z;
							}
							solCx( pos, eta_a, eta_b, x_c, (PetscInt)nz, vel, &pressure, total_stress, strain_rate );
							Vz_an = vel[1];
						}
						else {
							// In case we INTERPOLATE the FDSTAG velocity solution back to corner nodes, we have to use the corner
							// location
							// Vx and Vz velocity
							pos[0] = x; pos[1]=z;		// default is in x-z direction
							if (SolCx_y_direction){
								// SolCx in y-direction
								pos[0] = y; pos[1]=z;
							}
							solCx( pos, eta_a, eta_b, x_c, (PetscInt)nz, vel, &pressure, total_stress, strain_rate );
							Vx_an = vel[0];
							Vz_an = vel[1];

						}



						/* Evaluate pointwise error */
						Vx_error = PetscAbs(Vx_num-Vx_an);
						Vy_error = PetscAbs(Vy_num-0);
						Vz_error = PetscAbs(Vz_num-Vz_an);

						if (SolCx_y_direction){
							Vx_error = PetscAbs(Vx_num-0);
							Vy_error = PetscAbs(Vy_num-Vx_an);
							Vz_error = PetscAbs(Vz_num-Vz_an);
						}

						if (InterpolatedVelocity==0){
							// Only evaluate the error on non-ghost points FDSTAG has additional points on the side boundaries
							if ( (zs+zm==nnode_z) && (iz==nnode_z-1)){  // the local processor borders this boundary
								Vx_error = 0;	Vx_an=0;
							}
							if ( (ys+ym==nnode_y) && (iy==nnode_y-1)){
								Vx_error = 0;	Vx_an=0;
							}
							if ( (zs+zm==nnode_z) && (iz==nnode_z-1)){
								Vy_error = 0;
							}
							if ( (xs+xm==nnode_x) && (ix==nnode_x-1)){
								Vy_error = 0;
							}
							if ( (ys+ym==nnode_y) && (iy==nnode_y-1)){
								Vz_error = 0;	Vz_an=0;
							}
							if ( (xs+xm==nnode_x) && (ix==nnode_x-1)){
								Vz_error = 0; 	Vz_an=0;
							}
						}

					}

					BenchmarkType 	= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block test. Since we have no analytical solution, the error is zero.
						Vx_error = 0;
						Vy_error = 0;
						Vz_error = 0;
					}
					BenchmarkType 		= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_VerticalDensityCollumn",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Benchmark_VerticalDensityCollumn test. Since we have no analytical solution, the error is zero.
						Vx_error = 0;
						Vy_error = 0;
						Vz_error = 0;
					}

					/* Integrate values to compute L1 error */
					Volume_local = Volume_local + dx*dy*dz;
					Vx_err_int 	 = Vx_err_int 	+ Vx_error*dx*dy*dz;
					Vy_err_int 	 = Vy_err_int 	+ Vy_error*dx*dy*dz;
					Vz_err_int 	 = Vz_err_int 	+ Vz_error*dx*dy*dz;

					if (Benchmark_PlotErrors_VP){
						// for debugging ONLY!:
						velocity[iz][iy][ix].Vx = Vx_error;
						velocity[iz][iy][ix].Vy = Vy_error;
						velocity[iz][iy][ix].Vz = Vz_error;

						//velocity[iz][iy][ix].Vx = Vx_an;
						//velocity[iz][iy][ix].Vy = 0;
						//velocity[iz][iy][ix].Vz = Vz_an;
					}
					/* for debugging: print data to screen */


					//PetscPrintf(PETSC_COMM_SELF,"coord=[%g,%g,%g] V_num=[%g,%g,%g] V_an=[%g,%g] V_error=[%g,%g] \n", x,y,z, Vx_num, Vy_num, Vz_num, vel[0], vel[1], Vx_err_int, Vz_err_int);
					//PetscPrintf(PETSC_COMM_WORLD,"coord=[%g,%g,%g] P_num=[%g] P_an=[%g] E_num=[%g,%g,%g,%g,%g,%g] E_an=[%g,%g,%g] E_error=%g \n", x,y,z, P_num, pressure, Exx_num, Eyy_num, Ezz_num, Exy_num, Exz_num, Eyz_num, strain_rate[0], strain_rate[1], strain_rate[2], E_error);


					/* Further benchmarks can be added here, according to the scheme above */



				}
			}
		}


		/* Restore coordinates and velocity */
		ierr = DMDAVecRestoreArray(user->DA_Vel, coord,&coords); CHKERRQ(ierr);
		ierr = DMDAVecRestoreArray(user->DA_Vel, sol, &velocity); 	CHKERRQ(ierr);


		/* collect data from all procs */
		ierr = MPI_Allreduce(&Volume_local,	&Volume,		1, MPI_DOUBLE, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&Vx_err_int,		&Vx_error,		1, MPI_DOUBLE, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&Vy_err_int,		&Vy_error,		1, MPI_DOUBLE, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&Vz_err_int,		&Vz_error,		1, MPI_DOUBLE, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		Vx_error 	= Vx_error/Volume;
		Vy_error 	= Vy_error/Volume;
		Vz_error 	= Vz_error/Volume;
		V_error 	= Vx_error + Vy_error + Vz_error;


		/* Compute the pressure error */

		/* Get the coordinates from the DMDA */
		ierr = DMGetCoordinateDM(user->DA_Pres,		&cda); 											CHKERRQ(ierr);	// Coordinates
		ierr = DMGetCoordinatesLocal(user->DA_Pres,	&coord); 										CHKERRQ(ierr);
		ierr = DMDAVecGetArray(cda,coord,&coords); 																CHKERRQ(ierr);


		ierr = DMGetLocalVector(user->DA_Pres,&local_P); CHKERRQ(ierr);
		ierr = DMGlobalToLocalBegin(user->DA_Pres, Pressure, INSERT_VALUES, local_P); CHKERRQ(ierr);
		ierr = DMGlobalToLocalEnd(user->DA_Pres,   Pressure, INSERT_VALUES, local_P); CHKERRQ(ierr);
		ierr = DMDAVecGetArray(user->DA_Pres, local_P,	&P); CHKERRQ(ierr);

		ierr = DMDAGetCorners (user->DA_Pres,   &xs, &ys, &zs, &xm, &ym, &zm); CHKERRQ(ierr);


		/* Spacing for element (assumed to be constant in domain!) */
		dx 	= coords[zs+1][ys+1][xs+1].x-coords[zs][ys][xs].x;
		dy 	= coords[zs+1][ys+1][xs+1].y-coords[zs][ys][xs].y;
		dz 	= coords[zs+1][ys+1][xs+1].z-coords[zs][ys][xs].z;


		/* Loop over all nodes and evaluate error in velocity */
		ierr = DMDAVecGetArray(user->DA_Materials, user->Materials, &materials_array); CHKERRQ(ierr);
		Volume_local = 0;P_err_int=0;  P_err2_int = 0.0; E_err_int=0.0; P_err2_int = 0.0;
		for (iz=zs; iz<zs+zm; iz++){
			for (iy=ys; iy<ys+ym; iy++){
				for (ix=xs; ix<xs+xm; ix++){


					ierr = LaMEMSetMaterialDataMemoryFromArray( &material_data, ix-xs,iy-ys,iz-zs, C->ngp_vel, materials_array ); CHKERRQ(ierr);


					/* retrieve numerical parameters for the current nodal point */
					intp 	=	0;
					x 		= coords[iz][iy][ix].x;
					y 		= coords[iz][iy][ix].y;
					z 		= coords[iz][iy][ix].z;
					P_num 	= P[iz][iy][ix];
					Exx_num =	material_data.DevStrainrate[0][intp];
					Eyy_num =	material_data.DevStrainrate[1][intp];
					Ezz_num =	material_data.DevStrainrate[2][intp];
					Exz_num =	material_data.DevStrainrate[3][intp];
					Exy_num =	material_data.DevStrainrate[4][intp];
					Eyz_num =	material_data.DevStrainrate[5][intp];

					Sxx_num =	material_data.DevStress[0][intp] - P_num;
					Syy_num =	material_data.DevStress[1][intp] - P_num;
					Szz_num =	material_data.DevStress[2][intp] - P_num;
					Sxz_num =	material_data.DevStress[3][intp];
					Sxy_num =	material_data.DevStress[4][intp];
					Syz_num =	material_data.DevStress[5][intp];


					/* SolCx benchmark
					 */
					BenchmarkType 		= PETSC_FALSE;
					SolCx_y_direction 	= PETSC_FALSE;
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_SolCx",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					ierr 	 			= PetscOptionsGetBool( PETSC_NULL, "-SolCx_y_direction",   &SolCx_y_direction, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){

						/* Vx velocity*/
						pos[0] = x; pos[1]=z;		// default is in x-z direction
						if (SolCx_y_direction){
							// SolCx in y-direction
							pos[0] = y; pos[1]=z;
						}
						solCx( pos, eta_a, eta_b, x_c, (PetscInt)nz, vel, &pressure, total_stress, strain_rate );
						P_an = pressure;

						/* Evaluate pointwise error */
						P_error 	= 	PetscAbs(P_num-P_an);
						P_error2 	=	(P_num-P_an)*(P_num-P_an);

						E_error = PetscAbs(Exx_num-strain_rate[0]) 	+  PetscAbs(Ezz_num-strain_rate[1]) 	+  0*PetscAbs(Exz_num-strain_rate[2]) +
												PetscAbs(Eyy_num-0) 				+  PetscAbs(Eyz_num-0) 					+  PetscAbs(Exy_num-0) ;																// Strainrate


						S_error = PetscAbs(Sxx_num-total_stress[0]) +  PetscAbs(Szz_num-total_stress[1]) 	+  0*PetscAbs(Sxz_num-total_stress[2]) +
												PetscAbs(Syy_num-0) 				+  PetscAbs(Syz_num-0) 					+  PetscAbs(Sxy_num-0) ;																// total stress  [not sure whether this is correct in SolCx, so I'll leave it out]

						E_error = 0;	// strain rates @ integration points are not correctly initialized
						S_error = 0;
					}
					BenchmarkType 		= PETSC_FALSE;
					ierr 	 		= PetscOptionsGetBool( PETSC_NULL, "-Benchmark_FallingBlock",   &BenchmarkType, PETSC_NULL); CHKERRQ(ierr);
					if (BenchmarkType){
						// Falling block test. Since we have no analytical solution, the error is zero.
						P_error  	= 	0;
						P_error2 	= 	0;
						E_error 	=	0;
						S_error 	=	0;
					}

					/* Integrate values to compute L1 error */
					Volume_local = Volume_local + dx*dy*dz;
					P_err_int 	 = P_err_int 	+ P_error*dx*dy*dz;
					P_err2_int 	 = P_err2_int 	+ P_error2*dx*dy*dz;
				//	E_err_int 	 = E_err_int 	+ E_error *material_data.ElementVolume[intp];
				//	S_err_int 	 = S_err_int 	+ S_error *material_data.ElementVolume[intp];

					if (Benchmark_PlotErrors_VP){
						// for debugging ONLY!:
						P[iz][iy][ix] = P_error;
					}

					/* for debugging: print data to screen */
					//	PetscPrintf(PETSC_COMM_WORLD,"coord=[%g,%g,%g] P_num=[%g] P_an=[%g,%g] P_error=%g\n", x,y,z, P_num, P_an, P[iz][iy][ix], P_error);



					/* Further benchmarks can be added here, according to the scheme above */




				}
			}
		}

		/* restore array */
		ierr = DMDAVecRestoreArray(user->DA_Materials, user->Materials, &materials_array); CHKERRQ(ierr);

		/* Restore coordinates and velocity */
		ierr = DMDAVecRestoreArray(cda,coord,&coords); CHKERRQ(ierr);
//		ierr = VecDestroy(coord); CHKERRQ(ierr);
//		ierr = DMDestroy(cda); CHKERRQ(ierr);


		ierr = MPI_Allreduce(&P_err_int,		&P_error,		1, MPI_DOUBLE, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&P_err2_int,		&P_error2,		1, MPI_DOUBLE, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&Volume_local,	&Volume,		1, MPI_DOUBLE, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		ierr = MPI_Allreduce(&E_err_int,		&E_error,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);
		//ierr = MPI_Allreduce(&S_err_int,		&S_error,		1, MPIU_SCALAR, MPI_SUM, PETSC_COMM_WORLD); CHKERRQ(ierr);

		/* Volume average */
		P_error 	= 	P_error/Volume;
		P_error2 	=	P_error2/Volume;
		E_error 	= 	E_error/Volume;
		//S_error 	= 	S_error/Volume;



		ierr = DMDAVecRestoreArray(user->DA_Pres,   local_P, &P); 	CHKERRQ(ierr);
		ierr = DMLocalToGlobalBegin(user->DA_Pres,local_P,INSERT_VALUES,Pressure);CHKERRQ(ierr);
		ierr = DMLocalToGlobalEnd  (user->DA_Pres,local_P,INSERT_VALUES,Pressure);CHKERRQ(ierr);
		ierr = DMRestoreLocalVector(user->DA_Pres,&local_P); CHKERRQ(ierr);


	}


	/* Print results to the screen */
	PetscPrintf(PETSC_COMM_WORLD,"\n       dx = %g        [node spacing in x-direction   ]\n", dx);
	PetscPrintf(PETSC_COMM_WORLD,"       dy = %g        [node spacing  in y-direction   ] \n", dy);
	PetscPrintf(PETSC_COMM_WORLD,"       dz = %g        [node spacing  in z-direction   ] \n\n", dz);

	PetscPrintf(PETSC_COMM_WORLD,"  |Vx |_1 = %g \n", Vx_error);
	PetscPrintf(PETSC_COMM_WORLD,"  |Vy |_1 = %g \n", Vy_error);
	PetscPrintf(PETSC_COMM_WORLD,"  |Vz |_1 = %g \n\n", Vz_error);
	PetscPrintf(PETSC_COMM_WORLD,"  |u  |_1 = %g   [velocity                      ]\n", V_error  );
	PetscPrintf(PETSC_COMM_WORLD,"  |P  |_1 = %g   [pressure                      ]\n", P_error  );
	PetscPrintf(PETSC_COMM_WORLD,"  |e  |_1 = %g   [strain rate                   ]\n", E_error  );
	PetscPrintf(PETSC_COMM_WORLD,"  |P  |_2 = %g   [pressure                      ]\n", P_error2 );

	PetscPrintf(PETSC_COMM_WORLD,"\n --------------------------------------------------------------------------------  \n\n");


PetscFunctionReturn(0);
}
/*==============================================================================================================================*/


/*
PetscInt main( PetscInt argc, char **argv )
{
	PetscInt i,j;
	double pos[2], vel[2], pressure, total_stress[3], strain_rate[3];
	double x,z;


	for (i=0;i<101;i++){
		for(j=0;j<101;j++){
			x = i/100.0;
			z = j/100.0;

			pos[0] = x;
			pos[1] = z;
		//	solCx( pos, 1.0, 1000.0, 0.5, 1, vel, &pressure, total_stress, strain_rate );
		//	solCx( pos, 1.0, 1000000.0, 0.75, 3, vel, &pressure, total_stress, strain_rate );
			solCx( pos, 1.0, 1.0e2, 0.5, 3, vel, &pressure, total_stress, strain_rate );

		//	printf("t_xx, t_xz fucked !! \n");
			printf("%0.7f %0.7f %0.7f %0.7f %0.7f %0.7f %0.7f %0.7f %0.7f %0.7f %0.7f \n",
					pos[0],pos[1],
					vel[0],vel[1], pressure,
					total_stress[0], total_stress[1], total_stress[2],
					strain_rate[0], strain_rate[1], strain_rate[2] );
		}
		printf("\n");
	}


	return 0;
}
*/





/* --------------------------------------------------------------------------------------------------------------------------------------
 * solCx analytical solution
 *
 */
void solCx(
		double pos[],
		double _eta_A, double _eta_B, 	/* Input parameters: density, viscosity A, viscosity B */
		double _x_c, PetscInt _n, 			/* Input parameters: viscosity jump location, wavenumber in z */
		double vel[], double* presssure,
		double total_stress[], double strain_rate[] )
{
	double Z,u1,u2,u3,u4,u5,u6,ZA,ZB;
	double sum1,sum2,sum3,sum4,sum5,sum6,x,z,xc;
	double _C1A,_C2A,_C3A,_C4A,_C1B,_C2B,_C3B,_C4B,_C1,_C2,_C3,_C4;
	PetscInt n,nx;

	double t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20,t21,t22,t23,t24,t25,t26,t27,t28,t29,t30,t31,t32,t33,t34,t35,t36,t37,t38,t39,t40;
	double t41,t42,t43,t44,t45,t46,t47,t48,t49,t50,t51,t52,t53,t54,t55,t56,t57,t58,t59,t60,t61,t62,t63,t64,t65,t66,t67,t68,t69,t70,t71,t72,t73,t74,t75,t76,t77,t78,t79,t80;
	double t81,t82,t83,t84,t85,t86,t87,t88,t89,t90,t91,t92,t93,t94,t95,t96,t97,t98,t99,t100,t101,t102,t103,t104,t105,t106,t107,t108,t109,t110,t111,t112,t113,t115,t116,t117,t118,t119,t120;
	double t121,t122,t123,t124,t125,t126,t127,t128,t129,t130,t131,t132,t133,t134,t135,t136,t137,t138,t139,t140,t141,t142,t143,t144,t145,t146,t147,t148,t149,t150,t151,t152,t153,t154,t155,t156,t157,t158,t159,t160;
	double t161,t162,t163,t164,t165,t166,t167,t168,t169,t170,t171,t172,t173,t174,t175,t176,t177,t178,t179,t180,t181,t182,t183,t184,t186,t187,t188,t189,t190,t191,t192,t193,t194,t195,t196,t197,t198,t199;
	double t201,t202,t203,t204,t206,t207,t208,t209,t210,t211,t212,t213,t215,t216,t217,t218,t219,t220,t221,t222,t223,t224,t225,t226,t227,t228,t229,t230,t231,t232,t233,t234,t235,t236,t237,t238,t239,t240;
	double t241,t242,t243,t244,t245,t246,t247,t248,t249,t250,t251,t252,t253,t254,t255,t256,t257,t258,t259,t260,t261,t262,t263,t264,t265,t267,t268,t269,t270,t272,t273,t274,t275,t276,t277,t278,t279,t280;
	double t281,t282,t283,t284,t285,t286,t288,t289,t290,t291,t292,t295,t296,t297,t298,t299,t300,t301,t303,t304,t305,t307,t308,t310,t311,t312,t313,t314,t315,t316,t317,t318,t319,t320;
	double t321,t322,t323,t324,t325,t326,t327,t328,t329,t330,t331,t332,t334,t335,t336,t337,t338,t339,t340,t341,t342,t344,t345,t346,t347,t348,t349,t350,t351,t352,t353,t354,t355,t356,t358,t359,t360;
	double t361,t362,t363,t364,t365,t366,t367,t368,t369,t370,t371,t372,t373,t374,t375,t376,t377,t378,t379,t380,t381,t382,t383,t384,t385,t386,t387,t389,t390,t391,t393,t394,t395,t396,t397,t398;
	double t401,t402,t403,t404,t405,t406,t407,t408,t409,t410,t411,t412,t413,t414,t415,t416,t417,t418,t419,t421,t422,t423,t424,t425,t426,t427,t428,t429,t430,t431,t432,t433,t434,t436,t437,t438,t439,t440;
	double t441,t442,t443,t444,t445,t446,t447,t448,t450,t451,t453,t454,t455,t456,t457,t458,t459,t461,t462,t463,t464,t465,t466,t468,t469,t470,t471,t474,t475,t478,t480;
	double t482,t483,t484,t485,t488,t489,t490,t492,t493,t495,t497,t498,t499,t501,t502,t503,t504,t505,t507,t508,t509,t510,t511,t512,t513,t515,t518,t520;
	double t522,t525,t527,t528,t529,t530,t532,t533,t534,t535,t536,t538,t539,t541,t542,t544,t545,t546,t547,t548,t549,t550,t551,t552,t553,t554,t555,t556,t557,t560;
	double t561,t562,t563,t564,t567,t568,t571,t573,t575,t576,t578,t579,t583,t590,t591,t594,t595,t596,t597,t598,t600;
	double t601,t602,t604,t606,t607,t608,t611,t613,t615,t616,t617,t619,t621,t623,t624,t625,t626,t627,t629,t630,t632,t633,t634,t638,t639,t640;
	double t641,t642,t643,t644,t645,t647,t648,t649,t650,t651,t652,t653,t654,t655,t656,t657,t658,t659,t660,t662,t663,t665,t666,t667,t668,t670,t671,t672,t673,t674,t675,t676,t679,t680;
	double t682,t683,t684,t685,t686,t688,t689,t690,t691,t693,t694,t695,t696,t697,t698,t699,t700,t701,t702,t704,t705,t708,t709,t711,t712,t713,t714,t717,t718,t719;
	double t721,t722,t723,t726,t727,t728,t730,t733,t734,t735,t736,t737,t738,t739,t740,t741,t744,t745,t746,t749,t750,t752,t753,t754,t755,t757,t758,t759,t760;
	double t761,t762,t763,t764,t766,t767,t768,t770,t771,t772,t773,t774,t775,t776,t777,t778,t780,t781,t782,t785,t786,t789,t790,t791,t792,t793,t794,t795,t796,t797,t798,t800;
	double t801,t806,t807,t808,t809,t811,t812,t817,t818,t819,t821,t822,t824,t827,t828,t830,t834,t835,t837,t840;
	double t842,t843,t844,t845,t846,t849,t850,t853,t854,t855,t857,t858,t859,t860,t863,t864,t867,t868,t869,t873,t874,t877,t878,t879,t880;
	double t884,t888,t891,t894,t900,t901,t903,t904,t907,t908,t909,t911,t914,t915,t916,t919,t920;
	double t923,t924,t925,t926,t927,t929,t932,t935,t937,t939,t942,t943,t944,t945,t947,t948,t949,t950,t952,t953,t954,t955,t956,t957;
	double t961,t964,t965,t966,t967,t968,t969,t971,t972,t974,t977,t978,t981,t983,t987,t988,t992,t993,t994,t997,t998;
	double t1001,t1003,t1005,t1006,t1009,t1010,t1012,t1013,t1015,t1016,t1017,t1018,t1020,t1021,t1029,t1031,t1032,t1033,t1040;
	double t1041,t1042,t1044,t1047,t1050,t1054,t1055,t1057,t1058,t1063,t1068,t1069,t1070,t1079,t1080;
	double t1088,t1089,t1091,t1092,t1094,t1096,t1101,t1102,t1103,t1104,t1105,t1108,t1112,t1113,t1118,t1119,t1120;
	double t1121,t1122,t1123,t1124,t1125,t1126,t1127,t1128,t1129,t1130,t1132,t1133,t1134,t1135,t1138,t1139,t1140,t1141,t1142,t1145,t1146,t1148,t1149,t1150,t1153,t1154,t1156,t1157,t1158,t1159;
	double t1161,t1162,t1165,t1166,t1170,t1171,t1172,t1173,t1175,t1176,t1178,t1180,t1181,t1182,t1185,t1189,t1192,t1193,t1195,t1196,t1199;
	double t1201,t1203,t1209,t1210,t1211,t1213,t1214,t1218,t1221,t1224,t1225,t1226,t1228,t1233,t1234,t1235,t1236,t1237,t1240;
	double t1241,t1242,t1243,t1244,t1245,t1248,t1251,t1252,t1257,t1258,t1259,t1260,t1263,t1268,t1269,t1272,t1280;
	double t1282,t1283,t1284,t1285,t1287,t1288,t1289,t1292,t1293,t1296,t1297,t1300,t1304,t1307,t1310,t1311,t1312,t1316,t1317,t1320;
	double t1321,t1323,t1328,t1330,t1331,t1332,t1333,t1336,t1338,t1343,t1344,t1346,t1349,t1350,t1354;
	double t1366,t1369,t1370,t1371,t1376,t1378,t1380,t1383,t1386,t1387,t1388,t1391,t1393,t1399;
	double t1411,t1412,t1420,t1427;
	double t1450,t1456,t1468,t1472,t1474,t1478;
	double t1504,t1511;
	double t1545;
	double t1564,t1583;
	double PI=M_PI;


	/* del_rho = sin(n*Pi*z)*cos(nx*Pi*x)  n=nx gives only non-zero terms*/
	n = _n; /* only one n in Fourier series because  del_rho has cos term */
	nx = 1; /* can set these two to whatever we like */

	ZA=_eta_A; /* left column viscosity */
	ZB=_eta_B; /* right column viscosity */
	xc = _x_c;

	x = pos[0];
	z = pos[1];

	sum1=0.0;
	sum2=0.0;
	sum3=0.0;
	sum4=0.0;
	sum5=0.0;
	sum6=0.0;

	// Note that there is no Fourier sum here.
	/****************************************************************************************/
	_C1A = 0;
	/****************************************************************************************/
	t1 = nx * 0.3141592654e1;
	t2 = sin(t1);
	t3 = nx * t2;
	t4 = n * n;
	t5 = t4 * t4;
	t6 = 0.3141592654e1 * 0.3141592654e1;
	t8 = t3 * t5 * t6;
	t9 = ZA * xc;
	t12 = exp(xc * n * 0.3141592654e1);
	t13 = t12 * t12;
	t15 = n * 0.3141592654e1;
	t16 = exp(t15);
	t17 = t16 * t16;
	t18 = t17 * t16;
	t19 = ZB * t13 * t18;
	t20 = t9 * t19;
	t23 = ZA * ZA;
	t24 = nx * nx;
	t25 = t24 * nx;
	t26 = t23 * t25;
	t28 = t13 * t13;
	t29 = t28 * t13;
	t33 = nx * ZB;
	t34 = t1 * xc;
	t35 = sin(t34);
	t36 = t4 * n;
	t37 = t35 * t36;
	t38 = t33 * t37;
	t39 = 0.3141592654e1 * ZA;
	t40 = t13 * t12;
	t41 = t17 * t40;
	t45 = ZB * ZB;
	t46 = t45 * t24;
	t47 = t46 * t4;
	t48 = 0.3141592654e1 * xc;
	t49 = t13 * t17;
	t53 = xc * xc;
	t54 = t36 * t53;
	t56 = t54 * t6 * t45;
	t57 = cos(t34);
	t58 = t57 * t24;
	t59 = t28 * t12;
	t60 = t17 * t59;
	t61 = t58 * t60;
	t64 = t25 * t2;
	t65 = t64 * t15;
	t72 = nx * t23;
	t74 = t72 * t2 * t5;
	t75 = t6 * t53;
	t76 = t16 * t29;
	t80 = t23 * n;
	t81 = t80 * 0.3141592654e1;
	t82 = t18 * t28;
	t86 = nx * t5;
	t87 = t23 * t6;
	t89 = xc * t2;
	t90 = t13 * t18;
	t91 = t89 * t90;
	t94 = t28 * t28;
	t96 = t24 * n;
	t98 = t4 * t45;
	t99 = t98 * 0.3141592654e1;
	t100 = t58 * t41;
	t104 = 0.3141592654e1 * t25;
	t105 = ZA * n * t104;
	t106 = t2 * ZB;
	t110 = t17 * t17;
	t111 = ZA * t110;
	t116 = n * t28;
	t122 = t64 * t4 * t6;
	t126 = t23 * t29 * t4;
	t128 = t24 * xc;
	t132 = t36 * t23;
	t133 = t6 * t57;
	t135 = t128 * t41;
	t138 = t6 * xc;
	t142 = t72 * t2;
	t147 = 0.4e1 * t8 * t20 - 0.2e1 * t26 * t2 * t16 * t29 - 0.8e1 * t38 * t39 * t41 + 0.4e1 * t47 * t48 * t49 - 0.8e1 * t56 * t61 - 0.4e1 * t65 * t20 + 0.2e1 * t26 * t2 * t18 * t28 - 0.4e1 * t74 * t75 * t76 - 0.2e1 * t81 * t64 * t82 - 0.4e1 * t86 * t87 * t91 - t23 * t94 * t96 + 0.8e1 * t99 * t100 - 0.2e1 * t105 * t106 * t82 - 0.4e1 * t38 * t48 * t111 * t12 + 0.2e1 * t116 * ZB * t111 * t24 + 0.4e1 * t122 * t20 + 0.4e1 * t126 * 0.3141592654e1 * t17 * t128 + 0.8e1 * t132 * t133 * t135 + 0.4e1 * t74 * t138 * t76 - 0.2e1 * t142 * t4 * t18 * t28;
	t149 = ZA * t25 * t2;
	t150 = ZB * t28;
	t154 = t35 * t5;
	t155 = t72 * t154;
	t156 = t75 * t41;
	t159 = nx * ZA;
	t160 = t2 * t36;
	t161 = t159 * t160;
	t162 = 0.3141592654e1 * ZB;
	t163 = t28 * t16;
	t167 = t23 * t57;
	t168 = t167 * t24;
	t169 = n * t110;
	t170 = t169 * t40;
	t173 = ZA * ZB;
	t174 = t173 * t90;
	t177 = t36 * 0.3141592654e1;
	t181 = t80 * t104;
	t184 = n * t17;
	t188 = t17 * t29;
	t190 = t4 * 0.3141592654e1;
	t191 = t190 * t24;
	t206 = t138 * t60;
	t209 = t23 * t4;
	t211 = t209 * t6 * t25;
	t212 = t89 * t76;
	t216 = ZB * t16 * t29;
	t217 = t9 * t216;
	t220 = ZB * t110;
	t221 = ZA * t24;
	t222 = t221 * n;
	t225 = t132 * t75;
	t232 = t45 * t28;
	t233 = t110 * t24;
	t234 = t233 * n;
	t236 = t209 * 0.3141592654e1;
	t237 = t17 * xc;
	t239 = t237 * t13 * t24;
	t242 = -0.2e1 * t149 * t150 * t16 - 0.8e1 * t155 * t156 - 0.2e1 * t161 * t162 * t163 + 0.2e1 * t168 * t170 + 0.2e1 * t65 * t174 - 0.2e1 * t142 * t177 * t76 + 0.4e1 * t181 * t91 - 0.4e1 * t168 * t184 * t59 - 0.4e1 * t188 * t23 * t191 + 0.4e1 * t38 * t48 * ZA * t17 * t40 + 0.4e1 * t49 * t23 * t191 + 0.2e1 * t26 * t2 * t13 * t18 - 0.8e1 * t155 * t206 + 0.4e1 * t211 * t212 - 0.4e1 * t8 * t217 + 0.2e1 * t220 * t222 - 0.8e1 * t225 * t100 + 0.2e1 * t142 * t4 * t16 * t29 + t232 * t234 - 0.4e1 * t236 * t239;
	t244 = nx * t45;
	t245 = t244 * t37;
	t246 = t110 * t40;
	t251 = t237 * t59;
	t256 = t64 * t90;
	t260 = t36 * t45 * t133;
	t263 = t45 * t57;
	t264 = t263 * t24;
	t265 = t169 * t12;
	t269 = t6 * t36;
	t270 = t17 * t24;
	t274 = t110 * t13;
	t276 = t190 * t128;
	t279 = nx * t36;
	t281 = t28 * t40;
	t282 = t281 * t35;
	t286 = t138 * t41;
	t289 = t75 * t60;
	t296 = t190 * t173;
	t305 = t86 * t45 * t35;
	t312 = t33 * t154;
	t313 = t6 * ZA;
	t324 = t232 * t270;
	t327 = -0.2e1 * t245 * t48 * t246 + 0.4e1 * t159 * t37 * t162 * t251 + 0.4e1 * t209 * t75 * t256 + 0.8e1 * t260 * t135 + 0.2e1 * t264 * t265 + 0.32e2 * t9 * t150 * t269 * t270 + 0.4e1 * t274 * t23 * t276 + 0.2e1 * t279 * t45 * t282 * t48 + 0.8e1 * t155 * t286 + 0.8e1 * t155 * t289 - 0.8e1 * t150 * ZA * t96 * t17 + 0.8e1 * t296 * t61 - 0.2e1 * t105 * t106 * t163 - 0.2e1 * t81 * t256 - 0.8e1 * t305 * t156 - 0.4e1 * t33 * t282 * t177 * t9 - 0.16e2 * t312 * t313 * t237 * t40 - 0.4e1 * t168 * t184 * t40 + 0.2e1 * t168 * t265 + 0.16e2 * t269 * t53 * t324;
	t328 = t3 * t4;
	t331 = t72 * t37;
	t332 = t48 * t60;
	t335 = n * t94;
	t345 = t72 * t35;
	t349 = t173 * t57;
	t355 = t53 * t17;
	t364 = t54 * t6 * ZB;
	t365 = t28 * t17;
	t369 = xc * ZB;
	t370 = t269 * t369;
	t371 = ZA * t57;
	t373 = t371 * t270 * t40;
	t385 = nx * t35;
	t396 = t4 * xc;
	t397 = t396 * t162;
	t415 = t37 * t48;
	t418 = -0.32e2 * t364 * t365 * t221 - 0.16e2 * t370 * t373 - 0.4e1 * t331 * t48 * t41 + 0.4e1 * t86 * t23 * t53 * t6 * t2 * t90 + 0.2e1 * t385 * t177 * t23 * xc * t246 + 0.16e2 * t132 * t53 * t6 * t28 * t270 - 0.4e1 * t397 * t371 * t233 * t12 - 0.12e2 * t173 * t58 * t190 * t251 + 0.2e1 * t385 * t36 * 0.3141592654e1 * t23 * xc * t59 - 0.8e1 * t99 * t61 - 0.2e1 * t244 * t59 * t415;
	t427 = t371 * t270 * t59;
	t439 = t209 * t48;
	t440 = t110 * t12;
	t441 = t58 * t440;
	t447 = t36 * xc;
	t455 = t48 * t440;
	t471 = ZB * t17;
	t492 = 0.12e2 * t397 * t373 - 0.4e1 * t122 * t217 + 0.16e2 * t364 * t427 + 0.16e2 * t312 * t313 * t355 * t40 - 0.8e1 * t279 * t39 * t35 * ZB * t60 + 0.2e1 * t439 * t441 - 0.2e1 * t81 * t64 * t163 + 0.8e1 * t447 * t87 * t61 + 0.2e1 * t23 * t59 * t57 * t276 + 0.2e1 * t245 * t455 - 0.4e1 * t349 * t96 * t440 - 0.16e2 * t370 * t427 + 0.4e1 * t181 * t212 - 0.16e2 * t365 * t23 * t269 * t128 + 0.16e2 * t86 * t138 * ZA * t35 * t471 * t59 + 0.8e1 * t305 * t289 - 0.4e1 * t439 * t100 + 0.2e1 * ZB * t25 * t2 * ZA * t18 * t28 + 0.2e1 * t142 * t4 * t28 * t16 - 0.8e1 * t56 * t100;
	t499 = ZA * t53 * t19;
	t505 = t396 * 0.3141592654e1;
	t518 = t173 * t53 * t16 * t29;
	t533 = t23 * t28;
	t535 = t188 * t45;
	t538 = t24 * t4;
	t545 = t3 * t177;
	t546 = t173 * t76;
	t555 = t45 * t110;
	t557 = t72 * t160;
	t561 = -0.8e1 * t225 * t61 - 0.2e1 * t161 * t162 * t82 + t533 * t234 + 0.4e1 * t535 * t191 + 0.4e1 * t167 * t538 * t332 + 0.4e1 * t349 * t96 * t60 + 0.2e1 * t545 * t546 - 0.2e1 * t264 * t170 + 0.4e1 * t397 * t281 * ZA * t58 - t555 * t96 - 0.4e1 * t557 * t48 * t76;
	t567 = t396 * 0.3141592654e1 * t45;
	t568 = t58 * t246;
	t597 = t58 * n;
	t615 = t13 * t45;
	t616 = t615 * t233;
	t619 = t94 * t45;
	t621 = t45 * t59;
	t625 = 0.2e1 * t149 * t216 + 0.2e1 * t567 * t568 - 0.16e2 * t269 * xc * t324 - 0.2e1 * t236 * xc * t281 * t58 - 0.2e1 * t142 * t177 * t90 - 0.8e1 * t567 * t100 + 0.2e1 * t65 * t546 - 0.8e1 * t305 * t206 + 0.2e1 * n * t45 * t281 * t57 * t24 - t23 * t110 * t96 - 0.8e1 * t296 * t100 + 0.2e1 * t23 * t281 * t597 + 0.4e1 * t545 * t20 + 0.2e1 * t159 * t2 * t4 * ZB * t163 - 0.4e1 * t557 * t48 * t90 + 0.4e1 * t122 * t518 + 0.8e1 * t263 * t538 * t332 - 0.4e1 * t505 * t616 - t619 * t96 - 0.2e1 * t621 * t57 * t276;
	t626 = t49 * t45;
	t660 = t29 * t45;
	t685 = 0.2e1 * t545 * t174 - 0.4e1 * t126 * 0.3141592654e1 * t24 * xc - 0.4e1 * t47 * t48 * t188 + 0.4e1 * t505 * t660 * t24 - 0.2e1 * t142 * t177 * t163 - 0.2e1 * t142 * t4 * t13 * t18 + 0.8e1 * t260 * t128 * t60 - 0.2e1 * t328 * t546 - 0.2e1 * t26 * t2 * t28 * t16 + 0.4e1 * t545 * t217 - 0.4e1 * t209 * t138 * t256;
	t690 = t6 * 0.3141592654e1;
	t691 = ZA * t690;
	t693 = t24 * t24;
	t694 = t693 * xc;
	t695 = t188 * t694;
	t698 = t23 * ZA;
	t699 = t698 * t690;
	t700 = t699 * t5;
	t704 = t5 * t4;
	t705 = t691 * t704;
	t709 = t691 * t5;
	t713 = t5 * n;
	t714 = t713 * ZB;
	t718 = t698 * t6;
	t719 = t713 * t28;
	t722 = t699 * t704;
	t726 = t713 * t94;
	t733 = t713 * t45;
	t736 = t87 * t36;
	t740 = -0.4e1 * t691 * t98 * t695 + 0.8e1 * t700 * t270 * t13 + 0.4e1 * t705 * t660 * xc + 0.8e1 * t709 * t660 * t128 + 0.2e1 * t87 * t714 * t110 + t718 * t719 * t110 - 0.4e1 * t722 * t237 * t13 - t313 * t726 * t45 - 0.4e1 * t699 * t704 * xc * t29 + t313 * t733 * t28 + 0.4e1 * t736 * t150 * t233;
	t746 = t313 * t36;
	t752 = t6 * t6;
	t753 = t23 * t752;
	t759 = t698 * t752;
	t760 = t759 * t36;
	t761 = t17 * t693;
	t762 = xc * t28;
	t763 = t761 * t762;
	t766 = t87 * t713;
	t773 = t699 * t4;
	t774 = t110 * t693;
	t775 = xc * t13;
	t785 = t704 * t17;
	t789 = -0.16e2 * t736 * t150 * t270 + t718 * t116 * t693 - 0.2e1 * t746 * t555 * t24 + 0.4e1 * t705 * t535 + 0.64e2 * t753 * t713 * t17 * t150 * t128 - 0.16e2 * t760 * t763 + 0.2e1 * t766 * t150 * t110 + 0.4e1 * t722 * t274 * xc + 0.4e1 * t773 * t774 * t775 - 0.8e1 * t766 * t150 * t17 + 0.8e1 * t700 * t233 * t775 + 0.4e1 * t699 * t785 * t13;
	t791 = t691 * t4;
	t792 = t45 * t693;
	t793 = t49 * t792;
	t796 = t759 * t713;
	t797 = t53 * t28;
	t798 = t270 * t797;
	t801 = t87 * n;
	t818 = t5 * t36;
	t819 = t753 * t818;
	t827 = t753 * t36 * ZB;
	t830 = xc * t45;
	t834 = -0.4e1 * t791 * t793 + 0.32e2 * t796 * t798 + 0.2e1 * t801 * ZB * t693 * t110 + 0.2e1 * t718 * t36 * t28 * t24 - 0.8e1 * t700 * t128 * t29 - 0.8e1 * t700 * t239 - 0.8e1 * t801 * t150 * t761 + 0.32e2 * t819 * t365 * t369 - 0.64e2 * t753 * t714 * t798 + 0.32e2 * t827 * t763 + 0.4e1 * t705 * t830 * t49;
	t842 = xc * t29;
	t843 = t270 * t842;
	t849 = t759 * t818;
	t853 = t691 * t396;
	t857 = t691 * t5 * t45;
	t869 = t313 * n;
	t874 = -0.2e1 * t718 * t36 * t94 * t24 - 0.4e1 * t773 * t761 * t29 + 0.8e1 * t700 * t843 + 0.2e1 * t87 * t726 * ZB + 0.16e2 * t849 * t797 * t17 + 0.4e1 * t853 * t793 + 0.8e1 * t857 * t239 + 0.2e1 * t801 * t150 * t693 - 0.8e1 * t700 * t270 * t29 - 0.8e1 * t709 * t49 * t46 - t869 * t619 * t693 + t869 * t232 * t693;
	t877 = ZA * t752;
	t878 = t877 * t818;
	t911 = 0.16e2 * t878 * t53 * t45 * t365 - 0.4e1 * t699 * t785 * t29 - 0.4e1 * t705 * t188 * t830 + 0.2e1 * t801 * t94 * t693 * ZB - 0.8e1 * t857 * t843 - t718 * t726 + 0.4e1 * t773 * t761 * t13 - 0.4e1 * t705 * t775 * t555 + 0.2e1 * t746 * t232 * t233 - 0.16e2 * t878 * t830 * t365 - 0.2e1 * t746 * t619 * t24;
	t916 = t110 * t28;
	t945 = t28 * t693 * t45 * t17;
	t948 = 0.32e2 * t877 * t733 * t798 + 0.2e1 * t718 * t36 * t916 * t24 - 0.4e1 * t705 * t626 + t718 * n * t916 * t693 - t869 * t792 * t110 - 0.4e1 * t773 * t761 * t775 + t718 * t719 + 0.2e1 * t746 * t232 * t24 - 0.16e2 * t849 * t365 * xc - t718 * t713 * t110 - 0.4e1 * t773 * t694 * t29 + 0.16e2 * t877 * t54 * t945;
	t974 = t761 * t797;
	t987 = 0.4e1 * t773 * t695 + 0.4e1 * t736 * t150 * t24 + 0.4e1 * t722 * t842 * t17 - 0.16e2 * t877 * t447 * t945 + 0.2e1 * t87 * t714 * t28 + t313 * t713 * t916 * t45 - 0.4e1 * t853 * t615 * t774 - 0.32e2 * t877 * t713 * xc * t324 + 0.16e2 * t760 * t974 + 0.4e1 * t736 * t94 * t24 * ZB + t869 * t792 * t916 - 0.8e1 * t691 * t5 * xc * t616;
	t1021 = -t718 * t169 * t693 - 0.32e2 * t827 * t974 + 0.2e1 * t801 * t150 * t774 + 0.4e1 * t791 * t188 * t792 + 0.4e1 * t736 * t220 * t24 + 0.4e1 * t791 * t842 * t792 + 0.8e1 * t709 * t660 * t270 - t718 * t335 * t693 - 0.2e1 * t718 * t36 * t110 * t24 - 0.32e2 * t819 * t797 * t471 - t313 * t733 * t110 - 0.32e2 * t796 * t270 * t762;

	_C2A = (t147 - 0.4e1 * t65 * t217 + t418 + 0.2e1 * t150 * t222 + t327 - 0.2e1 * t149 * t19 + 0.2e1 * t335 * ZB * t24 * ZA - 0.16e2 * t312 * t313 * t355 * t59 - 0.4e1 * t281 * ZB * ZA * t597 - 0.2e1 * t505 * t45 * t281 * t58 - 0.4e1 * t211 * t2 * t53 * t76 + 0.8e1 * t305 * t286 - 0.4e1 * t122 * t499 - 0.4e1 * t331 * t332 + 0.8e1 * t345 * t177 * t60 - 0.2e1 * t142 * t177 * t82 + 0.2e1 * t72 * t281 * t415 + 0.4e1 * t349 * t96 * t41 - 0.2e1 * t81 * t64 * t76 + 0.2e1 * t58 * t80 * t59 + 0.8e1 * t345 * t177 * t41 - 0.4e1 * t8 * t499 + t242 + 0.4e1 * t8 * t518 + t625 + t685 + 0.2e1 * t328 * t174 + 0.2e1 * t331 * t455 - 0.2e1 * t33 * t2 * t4 * ZA * t82 - 0.4e1 * t626 * t191 + 0.16e2 * t364 * t373 - 0.2e1 * t621 * t597 - 0.2e1 * t439 * t568 + t492 + t533 * t96 + t232 * t96 + 0.2e1 * t567 * t441 + t561) / (t740 + t789 + t834 + t874 + t911 + t948 + t987 + t1021);
	/****************************************************************************************/
	t1 = n * n;
	t2 = t1 * n;
	t3 = t2 * 0.3141592654e1;
	t4 = t3 * xc;
	t5 = ZB * ZB;
	t7 = exp(n * 0.3141592654e1);
	t8 = t7 * t7;
	t9 = t5 * t8;
	t12 = exp(xc * n * 0.3141592654e1);
	t13 = t12 * t12;
	t14 = t13 * t13;
	t15 = t14 * t13;
	t19 = nx * nx;
	t21 = nx * 0.3141592654e1;
	t22 = sin(t21);
	t23 = t19 * nx * t22;
	t24 = t23 * 0.3141592654e1;
	t25 = ZA * ZB;
	t26 = t7 * t15;
	t27 = t25 * t26;
	t30 = t21 * xc;
	t31 = sin(t30);
	t32 = t31 * nx;
	t33 = t32 * n;
	t34 = ZA * ZA;
	t35 = t8 * t8;
	t36 = t34 * t35;
	t40 = t2 * t34;
	t41 = 0.3141592654e1 * t8;
	t42 = t41 * t15;
	t45 = t1 * t5;
	t46 = t14 * t14;
	t49 = t19 * t5;
	t51 = t19 * t46;
	t53 = t19 * t34;
	t55 = t8 * t7;
	t56 = t13 * t55;
	t57 = t25 * t56;
	t60 = t2 * nx;
	t61 = 0.3141592654e1 * 0.3141592654e1;
	t63 = t60 * t31 * t61;
	t64 = xc * xc;
	t65 = ZA * t64;
	t66 = ZB * t8;
	t67 = t14 * t12;
	t68 = t66 * t67;
	t69 = t65 * t68;
	t72 = -0.4e1 * t4 * t9 * t15 + 0.4e1 * t24 * t27 + 0.4e1 * t33 * t36 * t12 - 0.4e1 * t40 * t42 - t45 * t46 + t45 * t14 - t49 * t14 + t51 * t5 - t53 * t14 + 0.4e1 * t24 * t57 + 0.32e2 * t63 * t69;
	t73 = t1 * nx;
	t75 = t73 * t31 * 0.3141592654e1;
	t76 = t8 * t67;
	t77 = t25 * t76;
	t80 = t1 * t1;
	t81 = t80 * t34;
	t83 = t61 * t14;
	t87 = t1 * t19;
	t88 = cos(t30);
	t90 = t87 * t88 * t61;
	t91 = t5 * t64;
	t92 = t13 * t12;
	t93 = t8 * t92;
	t94 = t91 * t93;
	t100 = ZB * t64 * ZA * t8 * t92;
	t103 = n * t19;
	t105 = t103 * t88 * 0.3141592654e1;
	t106 = ZA * xc;
	t107 = ZB * t35;
	t109 = t106 * t107 * t12;
	t112 = t34 * xc;
	t113 = t112 * t93;
	t116 = t35 * t14;
	t118 = t1 * ZA;
	t119 = ZB * t14;
	t122 = t1 * t46;
	t125 = t19 * ZB;
	t126 = t35 * ZA;
	t127 = t125 * t126;
	t129 = t1 * ZB;
	t132 = -0.16e2 * t75 * t77 + 0.16e2 * t81 * t64 * t83 * t8 + 0.16e2 * t90 * t94 - 0.32e2 * t90 * t100 + 0.8e1 * t105 * t109 - 0.8e1 * t75 * t113 + t45 * t116 + 0.2e1 * t118 * t119 + 0.2e1 * t122 * t25 - 0.2e1 * t127 + 0.2e1 * t129 * t126;
	t134 = t1 * t34;
	t136 = t34 * t64;
	t137 = t136 * t76;
	t141 = t91 * t76;
	t145 = t103 * t34;
	t146 = 0.3141592654e1 * xc;
	t147 = t8 * t13;
	t153 = t14 * ZA;
	t156 = xc * t5;
	t157 = t156 * t93;
	t160 = t103 * t5;
	t162 = t146 * t8 * t15;
	t166 = t34 * t7 * t15;
	t169 = t134 * t116 - 0.16e2 * t63 * t137 - t49 * t116 - 0.16e2 * t63 * t141 - t53 * t116 + 0.4e1 * t145 * t146 * t147 - 0.2e1 * t51 * t25 - 0.2e1 * t125 * t153 - 0.16e2 * t75 * t157 + 0.4e1 * t160 * t162 - 0.4e1 * t24 * t166;
	t170 = t106 * t68;
	t177 = t35 * t92;
	t178 = t112 * t177;
	t181 = t156 * t76;
	t186 = t35 * t12;
	t187 = t112 * t186;
	t193 = t5 * 0.3141592654e1;
	t206 = t34 * t14;
	t207 = t206 * t7;
	t210 = -0.32e2 * t63 * t170 + 0.32e2 * t90 * t170 + 0.8e1 * t75 * t109 + 0.4e1 * t105 * t178 - 0.16e2 * t75 * t181 - 0.16e2 * t90 * t113 - 0.4e1 * t75 * t187 + 0.16e2 * t90 * t141 - 0.4e1 * t103 * t15 * t193 * xc + 0.16e2 * t73 * t22 * t34 * t146 * t26 + 0.4e1 * t32 * n * t34 * t67 + 0.4e1 * t24 * t207;
	t217 = t106 * t66 * t92;
	t226 = t88 * t19 * n;
	t227 = 0.3141592654e1 * t34;
	t229 = t227 * xc * t67;
	t232 = t73 * t31;
	t234 = t146 * t5 * t67;
	t238 = t61 * ZB;
	t239 = t14 * t8;
	t240 = t238 * t239;
	t243 = t136 * t93;
	t246 = -0.8e1 * t33 * t25 * t186 + 0.32e2 * t90 * t217 - t45 * t35 + t53 * t35 - t134 * t35 - t134 * t46 + t134 * t14 - 0.4e1 * t226 * t229 + 0.4e1 * t232 * t234 + 0.32e2 * t87 * t65 * t240 + 0.16e2 * t63 * t243;
	t247 = t14 * t92;
	t249 = t227 * t247 * xc;
	t254 = t73 * t22;
	t259 = t60 * t22 * t61;
	t260 = t112 * t26;
	t264 = t146 * t247 * t5;
	t268 = xc * t14;
	t274 = t5 * t14;
	t275 = t274 * t8;
	t280 = n * nx;
	t281 = t280 * t22;
	t282 = t55 * t14;
	t283 = t25 * t282;
	t290 = ZA * t247 * xc * ZB;
	t295 = t22 * nx * t1 * 0.3141592654e1;
	t298 = -0.4e1 * t232 * t249 + 0.8e1 * t105 * t217 - 0.4e1 * t254 * t227 * t26 - 0.8e1 * t259 * t260 - 0.4e1 * t232 * t264 - 0.16e2 * t81 * t61 * t268 * t8 + 0.16e2 * t80 * t64 * t61 * t275 - 0.4e1 * t232 * t229 + 0.8e1 * t281 * t283 - 0.4e1 * t105 * t187 + 0.8e1 * t75 * t290 + 0.4e1 * t295 * t27;
	t301 = t61 * t5;
	t307 = t87 * t34;
	t312 = t61 * xc;
	t313 = t312 * t239;
	t317 = t34 * t55 * t14;
	t329 = ZB * t13 * t55;
	t330 = t65 * t329;
	t337 = -0.16e2 * t87 * t64 * t301 * t239 - 0.32e2 * t90 * t69 - 0.16e2 * t307 * t64 * t61 * t239 + 0.16e2 * t307 * t313 + 0.4e1 * t24 * t317 + t53 * t46 + t49 * t35 - 0.32e2 * t63 * t100 - 0.4e1 * t280 * t31 * t34 * t247 + 0.8e1 * t259 * t330 - 0.4e1 * t280 * t31 * t247 * t5;
	t340 = t5 * t35;
	t344 = t25 * t93;
	t356 = t41 * t13;
	t360 = t23 * n * t61;
	t363 = t25 * t64 * t7 * t15;
	t366 = t156 * t177;
	t369 = t14 * t7;
	t370 = t25 * t369;
	t373 = t156 * t186;
	t378 = 0.4e1 * t24 * t283 + 0.4e1 * t33 * t340 * t12 - 0.16e2 * t75 * t344 - 0.4e1 * t280 * t31 * t5 * t67 + 0.8e1 * t33 * t25 * t247 + 0.32e2 * t63 * t217 + 0.4e1 * t40 * t356 - 0.8e1 * t360 * t363 + 0.4e1 * t75 * t366 + 0.4e1 * t295 * t370 - 0.4e1 * t75 * t373 - 0.4e1 * t105 * t366;
	t382 = t112 * t76;
	t387 = t80 * t61;
	t391 = t136 * t26;
	t409 = 0.16e2 * t63 * t382 + 0.4e1 * t226 * t234 - 0.16e2 * t387 * xc * t275 + 0.8e1 * t259 * t391 - 0.16e2 * t105 * t344 + 0.4e1 * t226 * t264 - 0.8e1 * t105 * t170 + 0.16e2 * t232 * t193 * t76 + 0.8e1 * t360 * t330 - 0.8e1 * t105 * t290 + 0.16e2 * t90 * t243;
	t423 = t153 * t8;
	t426 = t34 * t13;
	t427 = t426 * t55;
	t430 = t34 * t8;
	t437 = t80 * ZA;
	t441 = 0.4e1 * t145 * t42 - 0.16e2 * t90 * t157 + 0.24e2 * t75 * t217 + 0.4e1 * t226 * t249 + 0.4e1 * t254 * t227 * t282 + 0.4e1 * t160 * t356 - 0.8e1 * t129 * t423 - 0.8e1 * t281 * t427 - 0.8e1 * t33 * t430 * t67 + 0.8e1 * t33 * t430 * t92 + 0.32e2 * t437 * ZB * t313;
	t453 = t106 * ZB * t7 * t15;
	t456 = t2 * t5;
	t459 = t112 * t56;
	t462 = t126 * t14;
	t474 = t40 * 0.3141592654e1;
	t475 = xc * t8;
	t480 = t146 * t13 * t35;
	t483 = -0.4e1 * t103 * xc * t193 * t147 + 0.16e2 * t87 * t61 * t156 * t239 + 0.8e1 * t259 * t453 - 0.4e1 * t456 * t356 + 0.8e1 * t259 * t459 - 0.2e1 * t125 * t462 - 0.8e1 * t281 * t207 + 0.16e2 * t295 * t459 - 0.8e1 * t60 * t22 * ZA * t312 * t329 + 0.4e1 * t474 * t475 * t15 + 0.4e1 * t160 * t480;
	t497 = t136 * t56;
	t504 = t9 * t13;
	t509 = t475 * t13;
	t512 = -0.8e1 * t105 * t113 - 0.4e1 * t254 * t227 * t56 + 0.8e1 * t281 * t57 + 0.4e1 * t295 * t283 + 0.2e1 * t129 * t462 + 0.4e1 * t24 * t370 - 0.8e1 * t360 * t497 - 0.4e1 * t24 * t427 - 0.4e1 * t145 * t162 + 0.4e1 * t4 * t504 - 0.8e1 * t281 * t370 - 0.4e1 * t474 * t509;
	t528 = t5 * t13;
	t529 = t528 * t35;
	t532 = t106 * t329;
	t542 = -0.16e2 * t295 * t453 - 0.32e2 * t437 * t64 * t240 + 0.8e1 * t281 * t317 + 0.24e2 * t75 * t170 - 0.4e1 * t75 * t178 + 0.8e1 * t360 * t453 - 0.4e1 * t4 * t529 - 0.16e2 * t295 * t532 - 0.8e1 * t33 * t344 - 0.16e2 * t90 * t181 + 0.4e1 * t33 * t340 * t92;
	t557 = t146 * t15;
	t562 = xc * t15;
	t563 = t562 * t5;
	t573 = 0.16e2 * t232 * t193 * t93 - 0.8e1 * t259 * t363 - 0.8e1 * t259 * t497 + 0.8e1 * t33 * t77 + 0.8e1 * t360 * t391 + 0.4e1 * t254 * t227 * t369 + 0.4e1 * t145 * t557 + 0.8e1 * t281 * t166 + 0.4e1 * t3 * t563 + 0.8e1 * t105 * t382 - 0.4e1 * t145 * t480 - 0.4e1 * t33 * t36 * t92;
	t600 = 0.4e1 * t456 * t42 - 0.8e1 * t360 * t260 - 0.4e1 * t40 * t557 - 0.4e1 * t105 * t373 + 0.16e2 * t226 * t227 * t93 - 0.16e2 * t90 * t382 - 0.4e1 * t145 * t356 - 0.16e2 * t63 * t157 - 0.32e2 * t87 * t25 * t313 - 0.16e2 * t226 * t227 * t76 - 0.16e2 * t63 * t113;
	t623 = xc * t13;
	t627 = 0.8e1 * t125 * t423 - 0.8e1 * t360 * t532 + 0.16e2 * t90 * t137 - 0.4e1 * t160 * t42 + 0.16e2 * t63 * t94 + 0.16e2 * t63 * t181 - 0.8e1 * t281 * t27 - 0.8e1 * t75 * t382 + 0.8e1 * t360 * t459 + 0.4e1 * t295 * t57 + 0.16e2 * t105 * t77 + 0.4e1 * t474 * t623 * t35;
	t632 = t61 * 0.3141592654e1;
	t633 = t632 * t8;
	t634 = t80 * n;
	t638 = t632 * t634;
	t639 = t638 * xc;
	t642 = t61 * t34;
	t643 = t122 * t19;
	t649 = t61 * t61;
	t650 = t649 * t1;
	t652 = t19 * t19;
	t653 = t14 * t652;
	t654 = t653 * t9;
	t657 = t14 * t1;
	t658 = t657 * t19;
	t665 = t632 * t34;
	t666 = t665 * t2;
	t667 = t8 * t19;
	t668 = t667 * t623;
	t674 = t665 * n;
	t675 = t652 * xc;
	t682 = 0.8e1 * t633 * t426 * t634 - 0.8e1 * t639 * t529 - 0.4e1 * t642 * t643 + 0.2e1 * t642 * t116 * t80 + 0.32e2 * t650 * t64 * t654 + 0.4e1 * t301 * t658 + 0.4e1 * t387 * t46 * ZA * ZB - 0.16e2 * t666 * t668 - 0.16e2 * t666 * t667 * t15 - 0.8e1 * t674 * t675 * t15 + 0.4e1 * t238 * t153 * t80;
	t683 = t46 * t652;
	t686 = t633 * t15;
	t691 = t35 * t80;
	t698 = t35 * t652;
	t705 = t14 * t80;
	t708 = t61 * t35;
	t717 = -0.2e1 * t642 * t683 - 0.8e1 * t686 * t5 * t634 * xc - 0.2e1 * t301 * t691 + 0.8e1 * t638 * t563 - 0.2e1 * t642 * t691 - 0.2e1 * t642 * t698 - 0.2e1 * t301 * t698 - 0.2e1 * t301 * t683 + 0.2e1 * t642 * t705 + 0.2e1 * t708 * t274 * t80 + 0.2e1 * t301 * t653 - 0.2e1 * t642 * t80 * t46;
	t727 = t61 * t46;
	t737 = t649 * t34;
	t738 = t737 * t1;
	t739 = t8 * t652;
	t740 = t739 * t268;
	t746 = t61 * ZA;
	t754 = t632 * n * xc;
	t758 = 0.2e1 * t301 * t705 + 0.2e1 * t642 * t653 - 0.8e1 * t665 * xc * t634 * t15 - 0.2e1 * t727 * t5 * t80 - 0.32e2 * t650 * xc * t654 + 0.2e1 * t301 * t698 * t14 - 0.32e2 * t738 * t740 + 0.8e1 * t674 * t739 * t562 + 0.4e1 * t746 * t119 * t652 + 0.8e1 * t674 * t698 * t623 - 0.8e1 * t754 * t528 * t698;
	t762 = t633 * t13;
	t764 = t5 * n * t652;
	t767 = t80 * t1;
	t768 = t649 * t767;
	t772 = t649 * ZA;
	t773 = t772 * t129;
	t777 = t35 * t1 * t19;
	t780 = t632 * t5;
	t781 = t780 * t15;
	t786 = t698 * ZA;
	t790 = t64 * t14;
	t800 = t649 * t8;
	t809 = 0.4e1 * t238 * t126 * t80 - 0.8e1 * t762 * t764 - 0.32e2 * t768 * xc * t275 + 0.64e2 * t773 * t740 - 0.4e1 * t301 * t777 - 0.8e1 * t781 * n * t8 * t675 + 0.4e1 * t238 * t786 + 0.32e2 * t768 * t34 * t790 * t8 - 0.8e1 * t633 * t528 * t634 + 0.8e1 * t754 * t528 * t739 + 0.128e3 * t800 * t119 * t80 * t19 * t106 + 0.8e1 * t674 * t739 * t13;
	t812 = t649 * t80;
	t817 = t83 * ZB;
	t824 = t746 * ZB;
	t828 = t800 * t14;
	t855 = -0.64e2 * t812 * xc * t274 * t667 + 0.4e1 * t817 * t786 + 0.4e1 * t727 * ZA * t652 * ZB - 0.32e2 * t824 * t657 * t667 - 0.32e2 * t828 * t34 * t767 * xc - 0.8e1 * t633 * t15 * t34 * t634 - 0.8e1 * t674 * t739 * t15 + 0.32e2 * t768 * t64 * t275 + 0.4e1 * t708 * t14 * t307 + 0.2e1 * t708 * t206 * t652 + 0.8e1 * t632 * t35 * t13 * t34 * t634 * xc;
	t858 = t35 * t19;
	t873 = t2 * t8;
	t878 = t61 * t1;
	t901 = -0.16e2 * t632 * t2 * xc * t528 * t858 + 0.8e1 * t824 * t658 + 0.4e1 * t301 * t14 * t777 - 0.8e1 * t665 * t634 * t509 - 0.8e1 * t674 * t739 * t623 - 0.16e2 * t781 * t873 * t19 * xc + 0.8e1 * t878 * t14 * t127 + 0.8e1 * t878 * ZA * t51 * ZB + 0.8e1 * t686 * t764 + 0.8e1 * t665 * xc * t634 * t15 * t8 + 0.8e1 * t633 * t15 * t5 * t634 + 0.4e1 * t387 * t14 * t107 * ZA;
	t903 = t739 * t790;
	t923 = t737 * t80;
	t924 = t667 * t790;
	t927 = t780 * t2;
	t937 = t15 * t19 * xc;
	t943 = 0.32e2 * t738 * t903 + 0.16e2 * t781 * t873 * t19 + 0.8e1 * t754 * t15 * t652 * t5 + 0.16e2 * t666 * t858 * t623 + 0.64e2 * t828 * t25 * t767 * xc - 0.16e2 * t762 * t456 * t19 + 0.64e2 * t923 * t924 + 0.16e2 * t927 * t668 - 0.64e2 * t768 * ZA * t790 * t66 - 0.64e2 * t773 * t903 + 0.16e2 * t927 * t937 + 0.16e2 * t666 * t667 * t562;
	t977 = 0.64e2 * t812 * t5 * t924 + 0.8e1 * t639 * t504 + 0.8e1 * t238 * t35 * t118 * t19 + 0.4e1 * t642 * t658 - 0.16e2 * t817 * t437 * t8 - 0.128e3 * t772 * ZB * t80 * t924 + 0.16e2 * t666 * t667 * t13 - 0.4e1 * t301 * t643 - 0.16e2 * t824 * t653 * t8 - 0.4e1 * t642 * t777 - 0.64e2 * t923 * t667 * t268 - 0.16e2 * t666 * t937;

	_C3A = (t72 + t132 + t169 + t210 + t246 + t298 + t337 + t378 + t409 + t441 + t483 + t512 + t542 + t573 + t600 + t627) / (t682 + t717 + t758 + t809 + t855 + t901 + t943 + t977);
	/****************************************************************************************/
	_C4A = 0;
	/****************************************************************************************/
	t1 = nx * 0.3141592654e1;
	t2 = t1 * xc;
	t3 = cos(t2);
	t4 = nx * nx;
	t6 = n * 0.3141592654e1;
	t7 = t3 * t4 * t6;
	t8 = ZA * ZB;
	t9 = exp(t6);
	t10 = t9 * t9;
	t11 = xc * n;
	t13 = exp(t11 * 0.3141592654e1);
	t14 = t13 * t13;
	t15 = t14 * t13;
	t16 = t14 * t14;
	t17 = t16 * t15;
	t18 = t10 * t17;
	t19 = t8 * t18;
	t22 = sin(t2);
	t23 = nx * t22;
	t24 = t23 * n;
	t25 = ZB * ZB;
	t30 = n * n;
	t31 = t30 * n;
	t32 = t31 * nx;
	t33 = 0.3141592654e1 * 0.3141592654e1;
	t35 = t32 * t22 * t33;
	t36 = ZA * ZA;
	t37 = t36 * xc;
	t38 = t16 * t13;
	t39 = t10 * t38;
	t40 = t37 * t39;
	t43 = sin(t1);
	t44 = nx * t43;
	t45 = t30 * 0.3141592654e1;
	t46 = t44 * t45;
	t47 = ZA * xc;
	t49 = ZB * t16 * t9;
	t54 = t4 * nx * t43;
	t55 = xc * xc;
	t57 = t54 * t30 * t55;
	t58 = t33 * 0.3141592654e1;
	t59 = t58 * t25;
	t60 = t16 * t9;
	t61 = t59 * t60;
	t64 = xc * t25;
	t65 = t14 * t9;
	t66 = t64 * t65;
	t70 = t44 * t31 * t33;
	t71 = t37 * t65;
	t74 = t10 * t15;
	t75 = t64 * t74;
	t78 = t25 * t10;
	t83 = t54 * n * t33;
	t84 = t55 * t25;
	t85 = t10 * t9;
	t86 = t14 * t85;
	t87 = t84 * t86;
	t90 = t30 * t30;
	t92 = t44 * t90 * t58;
	t93 = t55 * xc;
	t94 = t93 * t25;
	t95 = t85 * t16;
	t96 = t94 * t95;
	t102 = t23 * t45;
	t103 = t10 * t10;
	t104 = ZB * t103;
	t106 = t47 * t104 * t15;
	t111 = t54 * 0.3141592654e1;
	t112 = t25 * t85;
	t113 = t112 * t16;
	t115 = t8 * t39;
	t118 = t16 * t14;
	t119 = t85 * t118;
	t120 = t37 * t119;
	t123 = t16 * t16;
	t124 = t36 * t123;
	t125 = t124 * t9;
	t127 = -0.8e1 * t7 * t19 + 0.2e1 * t24 * t25 * t13 * t10 - 0.16e2 * t35 * t40 - 0.16e2 * t46 * t47 * t49 - 0.8e1 * t57 * t61 + 0.4e1 * t46 * t66 + 0.2e1 * t70 * t71 - 0.16e2 * t35 * t75 + 0.6e1 * t24 * t78 * t38 - 0.2e1 * t83 * t87 - 0.8e1 * t92 * t96 - 0.8e1 * t46 * t37 * t95 - 0.12e2 * t102 * t106 + 0.2e1 * t83 * t71 + t111 * t113 + 0.8e1 * t7 * t115 + 0.2e1 * t83 * t120 + t111 * t125;
	t128 = t37 * t74;
	t131 = t44 * n;
	t133 = t25 * t9 * t118;
	t136 = t36 * t14;
	t137 = t136 * t9;
	t140 = t30 * t4;
	t142 = t140 * t3 * t33;
	t143 = t64 * t39;
	t147 = t30 * nx * t43;
	t148 = 0.3141592654e1 * t36;
	t149 = t9 * t118;
	t153 = t44 * t31 * ZA;
	t154 = t33 * xc;
	t155 = t154 * t49;
	t160 = ZA * t17 * xc * ZB;
	t163 = t103 * t13;
	t164 = t64 * t163;
	t170 = t44 * t90 * t55;
	t171 = t58 * ZB;
	t172 = ZA * t16;
	t174 = t171 * t172 * t9;
	t177 = t36 * t55;
	t178 = t177 * t149;
	t181 = t54 * t11;
	t182 = t33 * t25;
	t186 = t25 * t14;
	t187 = t186 * t9;
	t193 = t186 * t85;
	t198 = ZB * t55;
	t199 = ZA * t103;
	t201 = t198 * t199 * t15;
	t204 = 0.2e1 * t7 * t128 - 0.2e1 * t131 * t133 - 0.2e1 * t131 * t137 + 0.16e2 * t142 * t143 - t147 * t148 * t149 + 0.8e1 * t153 * t155 - 0.4e1 * t7 * t160 + 0.2e1 * t7 * t164 + 0.10e2 * t102 * t40 + 0.16e2 * t170 * t174 + 0.2e1 * t83 * t178 - 0.2e1 * t181 * t182 * t65 - t111 * t187 - 0.2e1 * t70 * t87 + 0.4e1 * t102 * t160 - 0.2e1 * t131 * t193 - 0.16e2 * t142 * t75 + 0.16e2 * t35 * t201;
	t210 = t32 * t22;
	t211 = t33 * t55;
	t212 = t25 * t38;
	t213 = t211 * t212;
	t216 = n * nx;
	t217 = t22 * t25;
	t222 = ZB * t85 * t16;
	t226 = t23 * t30;
	t227 = t13 * t10;
	t228 = t148 * t227;
	t233 = t37 * t163;
	t237 = n * t4 * t3;
	t238 = t148 * t74;
	t241 = t64 * t86;
	t245 = t148 * xc * t15;
	t248 = t112 * t118;
	t250 = t22 * t36;
	t256 = 0.3141592654e1 * t25;
	t257 = t256 * t39;
	t262 = t38 * t103;
	t263 = t37 * t262;
	t267 = t148 * t17 * xc;
	t270 = -0.6e1 * t7 * t143 - 0.4e1 * t24 * t19 - 0.8e1 * t210 * t213 - 0.2e1 * t216 * t217 * t15 - 0.32e2 * t153 * t211 * t222 + 0.4e1 * t226 * t228 + 0.16e2 * t142 * t201 + 0.2e1 * t7 * t233 - 0.4e1 * t237 * t238 - 0.2e1 * t83 * t241 - 0.2e1 * t237 * t245 + t111 * t248 + 0.2e1 * t216 * t250 * t15 - 0.2e1 * t131 * t125 - 0.4e1 * t226 * t257 + t147 * t148 * t95 - 0.2e1 * t102 * t263 + 0.2e1 * t237 * t267;
	t273 = t37 * t149;
	t277 = t47 * t104 * t13;
	t285 = t31 * t36;
	t286 = t44 * t285;
	t291 = t25 * t123 * t9;
	t304 = 0.3141592654e1 * xc;
	t305 = t304 * t212;
	t312 = t256 * t18;
	t315 = t8 * t60;
	t319 = t54 * t30 * t58;
	t323 = t90 * t36;
	t324 = t44 * t323;
	t325 = t55 * t58;
	t326 = t325 * t60;
	t329 = 0.2e1 * t102 * t164 + 0.2e1 * t83 * t273 - 0.4e1 * t102 * t277 - 0.2e1 * t7 * t263 + 0.4e1 * t24 * t8 * t17 - 0.4e1 * t286 * t154 * t60 - 0.2e1 * t131 * t291 - t147 * t148 * t119 + 0.2e1 * t24 * t78 * t17 + 0.2e1 * t54 * t85 * 0.3141592654e1 * ZA * ZB - 0.4e1 * t226 * t305 - 0.2e1 * t70 * t66 + t147 * t256 * t95 + 0.4e1 * t237 * t312 + 0.2e1 * t111 * t315 - 0.8e1 * t319 * t96 - t111 * t193 - 0.8e1 * t324 * t326;
	t332 = t8 * t95;
	t335 = t136 * t85;
	t337 = t256 * t227;
	t340 = t177 * t119;
	t346 = t37 * t86;
	t351 = t103 * t15;
	t352 = t177 * t351;
	t355 = t64 * t119;
	t358 = t8 * t227;
	t361 = t85 * 0.3141592654e1;
	t365 = t84 * t39;
	t372 = ZB * t10;
	t373 = t372 * t38;
	t374 = t47 * t373;
	t379 = t177 * t39;
	t384 = -0.2e1 * t46 * t332 + t111 * t335 + 0.4e1 * t237 * t337 - 0.2e1 * t83 * t340 + 0.16e2 * t286 * t211 * t95 + 0.2e1 * t70 * t346 - 0.8e1 * t170 * t61 - 0.8e1 * t142 * t352 - 0.2e1 * t83 * t355 - 0.4e1 * t24 * t358 + 0.2e1 * t147 * t361 * t8 + 0.8e1 * t35 * t365 - 0.2e1 * t226 * t267 + 0.8e1 * t102 * t115 - 0.12e2 * t102 * t374 + 0.16e2 * t142 * t40 - 0.8e1 * t142 * t379 + 0.4e1 * t237 * t228;
	t386 = t54 * t30 * t93;
	t387 = ZA * t85;
	t389 = t171 * t387 * t16;
	t394 = t64 * t60;
	t398 = t304 * t25 * t15;
	t401 = t361 * t25;
	t405 = t84 * t65;
	t410 = t148 * t18;
	t414 = t25 * t16 * t9;
	t417 = t84 * t74;
	t422 = t177 * t86;
	t428 = ZB * t38;
	t429 = t47 * t428;
	t432 = t148 * t39;
	t439 = 0.16e2 * t386 * t389 - 0.16e2 * t386 * t174 + 0.8e1 * t46 * t394 + 0.2e1 * t237 * t398 - t147 * t401 + 0.4e1 * t7 * t374 + 0.2e1 * t83 * t405 - 0.4e1 * t46 * t241 - 0.4e1 * t226 * t410 + 0.2e1 * t131 * t414 + 0.8e1 * t35 * t417 - 0.8e1 * t142 * t365 + 0.2e1 * t70 * t422 - 0.4e1 * t181 * t182 * t60 + 0.12e2 * t102 * t429 - 0.4e1 * t226 * t432 + 0.32e2 * t35 * t374 - 0.4e1 * t7 * t106;
	t442 = t36 * t9 * t118;
	t444 = t123 * t9;
	t445 = t8 * t444;
	t448 = t361 * t36;
	t451 = t47 * t372 * t17;
	t454 = t94 * t60;
	t457 = t25 * t103;
	t465 = t47 * t372 * t15;
	t468 = t36 * t85;
	t469 = t468 * t16;
	t474 = t43 * t85;
	t478 = t8 * t74;
	t484 = t256 * t74;
	t489 = t198 * ZA * t10 * t15;
	t501 = -t111 * t442 + 0.4e1 * t131 * t445 - t147 * t448 + 0.4e1 * t7 * t451 + 0.8e1 * t92 * t454 - 0.2e1 * t24 * t457 * t13 - 0.2e1 * t286 * t211 * t65 + 0.4e1 * t7 * t465 + t111 * t469 - 0.2e1 * t216 * t250 * t17 - 0.2e1 * t216 * t474 * t25 - 0.4e1 * t24 * t478 + 0.4e1 * t24 * t8 * t38 + 0.4e1 * t226 * t484 - 0.16e2 * t142 * t489 - 0.2e1 * t24 * t212 * t103 - 0.2e1 * t216 * t22 * t17 * t25 + 0.2e1 * t70 * t120;
	t504 = t33 * t36 * t55 * t38;
	t507 = t37 * t18;
	t512 = t47 * ZB * t13 * t10;
	t518 = t59 * t95;
	t530 = t84 * t351;
	t534 = t37 * t227;
	t549 = -0.8e1 * t210 * t504 + 0.2e1 * t102 * t507 + 0.4e1 * t7 * t512 + t111 * t133 - 0.16e2 * t35 * t489 + 0.8e1 * t170 * t518 + 0.2e1 * t24 * t36 * t13 * t10 + 0.4e1 * t131 * t387 * ZB + 0.12e2 * t102 * t465 - 0.8e1 * t142 * t530 + t111 * t291 - 0.2e1 * t102 * t534 - 0.4e1 * t70 * t394 - 0.10e2 * t102 * t128 + 0.4e1 * t237 * t305 + 0.8e1 * t102 * t19 + 0.2e1 * t83 * t346 - 0.16e2 * t35 * t128;
	t557 = t468 * t118;
	t562 = t93 * t58;
	t563 = t562 * t60;
	t567 = t44 * t90 * t93;
	t575 = ZA * t55;
	t576 = t575 * t428;
	t583 = t37 * t60;
	t590 = t140 * t3;
	t601 = -0.2e1 * t226 * t398 - 0.2e1 * t70 * t340 - 0.2e1 * t131 * t557 - 0.4e1 * t24 * t115 + 0.8e1 * t324 * t563 + 0.16e2 * t567 * t389 + 0.16e2 * t70 * t84 * t95 + 0.2e1 * t70 * t178 - 0.16e2 * t142 * t576 - 0.4e1 * t237 * t257 - 0.4e1 * t226 * t312 + 0.8e1 * t46 * t583 + 0.2e1 * t24 * t36 * t38 * t103 + 0.8e1 * t590 * t213 + 0.2e1 * t102 * t143 - 0.16e2 * t35 * t143 + 0.2e1 * t131 * t248 + 0.4e1 * t46 * t346;
	t604 = n * t36;
	t606 = t154 * t95;
	t625 = t36 * t103;
	t640 = t30 * t36;
	t641 = t54 * t640;
	t642 = t325 * t95;
	t647 = -0.4e1 * t131 * t315 - 0.4e1 * t54 * t604 * t606 - t147 * t148 * t60 + 0.16e2 * t35 * t576 - 0.8e1 * t102 * t478 + 0.32e2 * t142 * t465 - 0.4e1 * t237 * t484 - 0.2e1 * t70 * t355 + 0.2e1 * t70 * t273 + 0.2e1 * t102 * t233 - 0.2e1 * t24 * t625 * t13 - 0.8e1 * t7 * t358 - 0.2e1 * t111 * t445 - 0.4e1 * t7 * t429 + 0.16e2 * t46 * t47 * t222 + 0.2e1 * t131 * t113 + 0.8e1 * t641 * t642 - 0.2e1 * t7 * t534;
	t652 = t36 * t16;
	t653 = t652 * t9;
	t655 = t64 * t227;
	t658 = t182 * t95;
	t663 = t562 * t95;
	t684 = t64 * t351;
	t689 = t36 * t10;
	t695 = t154 * t222;
	t698 = -0.4e1 * t216 * t217 * t38 - t111 * t653 - 0.2e1 * t7 * t655 - 0.4e1 * t181 * t658 + 0.2e1 * t131 * t469 - 0.8e1 * t641 * t663 - 0.4e1 * t83 * t583 - 0.2e1 * t83 * t177 * t65 - 0.4e1 * t24 * t457 * t15 + 0.16e2 * t70 * t84 * t60 + 0.8e1 * t57 * t518 - 0.32e2 * t142 * t374 + 0.4e1 * t24 * t8 * t351 + 0.4e1 * t102 * t684 - t147 * t256 * t86 - 0.2e1 * t24 * t689 * t15 - 0.2e1 * t70 * t241 + 0.8e1 * t153 * t695;
	t711 = t575 * t373;
	t717 = t304 * t17 * t25;
	t736 = t177 * t74;
	t739 = 0.2e1 * t226 * t245 - 0.8e1 * t102 * t358 - 0.16e2 * t57 * t389 - 0.2e1 * t102 * t655 + 0.8e1 * t590 * t504 - 0.8e1 * t641 * t326 - 0.16e2 * t35 * t711 - t111 * t557 + t111 * t137 - 0.2e1 * t226 * t717 + 0.8e1 * t102 * t37 * t351 + 0.2e1 * t131 * t335 - 0.4e1 * t131 * t332 - 0.2e1 * t216 * t474 * t36 - 0.2e1 * t111 * t332 + 0.16e2 * t142 * t711 - t147 * t256 * t60 + 0.8e1 * t142 * t736;
	t750 = t64 * t262;
	t763 = t44 * t640;
	t770 = t84 * t119;
	t782 = 0.4e1 * t102 * t512 + 0.8e1 * t142 * t417 + 0.8e1 * t641 * t563 - 0.2e1 * t7 * t507 + 0.2e1 * t7 * t750 - 0.8e1 * t35 * t352 + 0.4e1 * t237 * t410 + 0.4e1 * t7 * t684 - 0.2e1 * t46 * t445 + t147 * t148 * t65 + 0.4e1 * t763 * t304 * t119 + 0.16e2 * t70 * t177 * t60 + 0.2e1 * t70 * t770 - t111 * t414 - 0.16e2 * t567 * t174 - 0.4e1 * t46 * t71 - 0.4e1 * t46 * t355 - 0.4e1 * t7 * t277;
	t797 = t64 * t149;
	t821 = -t54 * t448 + 0.2e1 * t131 * t442 + 0.8e1 * t7 * t478 + 0.8e1 * t35 * t379 - 0.2e1 * t181 * t182 * t149 + 0.2e1 * t70 * t405 + 0.2e1 * t83 * t770 - 0.2e1 * t70 * t797 - 0.6e1 * t7 * t75 - 0.4e1 * t286 * t606 - 0.4e1 * t237 * t432 + t147 * t256 * t149 - 0.4e1 * t763 * t304 * t149 - 0.2e1 * t102 * t75 + 0.2e1 * t237 * t717 + 0.8e1 * t324 * t642 - 0.16e2 * t170 * t389 + 0.2e1 * t83 * t422;
	t827 = t84 * t149;
	t846 = t54 * n * ZA;
	t854 = t64 * t18;
	t867 = -0.16e2 * t142 * t128 + 0.32e2 * t35 * t465 - 0.2e1 * t83 * t827 + 0.2e1 * t46 * t315 + t147 * t148 * t86 - 0.4e1 * t102 * t451 - 0.8e1 * t226 * t148 * xc * t38 - 0.2e1 * t24 * t689 * t38 + 0.2e1 * t131 * t187 + 0.8e1 * t846 * t155 + 0.8e1 * t35 * t736 + 0.2e1 * t24 * t689 * t17 - 0.2e1 * t7 * t854 + t147 * t256 * t119 + 0.2e1 * t102 * t854 - 0.8e1 * t35 * t530 + 0.4e1 * t46 * t797 + 0.2e1 * t102 * t750;
	t909 = -0.8e1 * t324 * t663 + t147 * t256 * t444 - t147 * t256 * t65 + 0.4e1 * t226 * t238 + 0.2e1 * t7 * t40 - t54 * t401 + 0.16e2 * t57 * t174 + 0.4e1 * t226 * t337 + 0.4e1 * t24 * t8 * t163 + 0.8e1 * t846 * t695 + 0.8e1 * t319 * t454 + 0.2e1 * t131 * t653 - 0.8e1 * t46 * t64 * t95 + 0.6e1 * t24 * t78 * t15 - 0.4e1 * t44 * t31 * xc * t658 - 0.32e2 * t153 * t211 * t49 - 0.2e1 * t70 * t827 + t147 * t148 * t444;
	t914 = t25 * ZB;
	t915 = t33 * t914;
	t919 = t4 * t4;
	t920 = t16 * t919;
	t929 = t123 * t90;
	t932 = t919 * t103;
	t935 = t33 * ZB;
	t939 = t652 * t919;
	t942 = t16 * t30;
	t943 = t942 * t4;
	t949 = t103 * t16;
	t950 = t949 * t90;
	t953 = -0.2e1 * t915 * t103 * t90 + 0.2e1 * t915 * t920 - 0.2e1 * t915 * t123 * t919 + 0.2e1 * t915 * t16 * t90 - 0.2e1 * t915 * t929 - 0.2e1 * t915 * t932 - 0.2e1 * t935 * t323 * t123 + 0.2e1 * t935 * t939 + 0.4e1 * t915 * t943 + 0.4e1 * t182 * t172 * t90 + 0.2e1 * t915 * t950;
	t954 = t171 * t36;
	t955 = t90 * n;
	t956 = xc * t955;
	t957 = t118 * t10;
	t964 = t33 * t33;
	t965 = t964 * ZB;
	t966 = t965 * t640;
	t967 = t10 * t919;
	t968 = t55 * t16;
	t969 = t967 * t968;
	t972 = t935 * t36;
	t974 = t103 * t30 * t4;
	t977 = xc * t16;
	t978 = t967 * t977;
	t981 = t90 * t30;
	t983 = t16 * t10;
	t987 = t182 * ZA;
	t988 = t4 * t10;
	t992 = t171 * t604;
	t993 = xc * t14;
	t994 = t932 * t993;
	t997 = t182 * t30;
	t1005 = t171 * t285;
	t1006 = t988 * t993;
	t1009 = t58 * t914;
	t1010 = t1009 * t31;
	t1013 = 0.8e1 * t954 * t956 * t957 + 0.2e1 * t915 * t932 * t16 + 0.32e2 * t966 * t969 - 0.4e1 * t972 * t974 - 0.32e2 * t966 * t978 + 0.32e2 * t965 * t981 * t177 * t983 - 0.32e2 * t987 * t942 * t988 + 0.8e1 * t992 * t994 + 0.8e1 * t997 * t949 * ZA * t4 - 0.2e1 * t935 * t124 * t919 - 0.16e2 * t1005 * t1006 + 0.16e2 * t1010 * t1006;
	t1015 = t964 * t25;
	t1016 = ZA * t30;
	t1017 = t1015 * t1016;
	t1020 = t967 * t993;
	t1031 = t1009 * t118;
	t1032 = t31 * t10;
	t1040 = t964 * t914;
	t1041 = t1040 * t90;
	t1044 = t55 * t10 * t4 * t16;
	t1047 = t1040 * t30;
	t1050 = t123 * ZA;
	t1054 = t977 * t988;
	t1057 = 0.64e2 * t1017 * t978 - 0.8e1 * t992 * t1020 + 0.2e1 * t972 * t950 + 0.4e1 * t182 * t929 * ZA + 0.4e1 * t182 * t199 * t90 - 0.16e2 * t1031 * t1032 * t4 * xc + 0.4e1 * t182 * t172 * t919 + 0.64e2 * t1041 * t1044 + 0.32e2 * t1047 * t969 + 0.4e1 * t182 * t1050 * t919 - 0.64e2 * t1041 * t1054;
	t1058 = t1009 * n;
	t1063 = t932 * ZA;
	t1069 = t123 * t30 * t4;
	t1080 = t993 * t103 * t4;
	t1088 = t935 * t103;
	t1094 = -0.8e1 * t1058 * t994 - 0.32e2 * t1047 * t978 + 0.4e1 * t182 * t1063 - 0.4e1 * t915 * t974 - 0.4e1 * t915 * t1069 - 0.2e1 * t935 * t625 * t90 - 0.8e1 * t1009 * t10 * t14 * t955 - 0.16e2 * t1010 * t1080 - 0.2e1 * t935 * t625 * t919 - 0.64e2 * t1017 * t969 + 0.2e1 * t1088 * t939 + 0.8e1 * t1009 * t957 * t955;
	t1113 = t955 * t118 * xc;
	t1120 = t4 * t118;
	t1125 = t981 * xc;
	t1133 = n * t10;
	t1140 = -0.8e1 * t954 * t955 * t10 * t993 + 0.2e1 * t935 * t652 * t90 - 0.64e2 * t1015 * t981 * t575 * t983 + 0.8e1 * t182 * t103 * t1016 * t4 + 0.8e1 * t1009 * t1113 + 0.16e2 * t954 * t1032 * t4 * t14 - 0.16e2 * t954 * t1032 * t1120 + 0.64e2 * t1015 * t10 * t172 * t1125 + 0.8e1 * t171 * t103 * t136 * t956 - 0.8e1 * t1031 * t1133 * t919 * xc + 0.8e1 * t1058 * t1020;
	t1153 = xc * t118;
	t1165 = t182 * t16;
	t1170 = t171 * t10;
	t1178 = ZA * t90;
	t1182 = 0.4e1 * t1088 * t652 * t140 + 0.8e1 * t954 * t1133 * t919 * t14 + 0.4e1 * t972 * t943 - 0.4e1 * t972 * t1069 - 0.16e2 * t954 * t31 * t4 * t1153 - 0.8e1 * t954 * n * t919 * t1153 - 0.8e1 * t954 * t1133 * t919 * t118 + 0.4e1 * t1165 * t1063 + 0.16e2 * t1005 * t1080 - 0.8e1 * t1170 * t118 * t36 * t955 - 0.16e2 * t987 * t920 * t10 - 0.16e2 * t1165 * t1178 * t10;
	t1195 = t1040 * t981;
	t1199 = t1009 * t955;
	t1203 = t1009 * t10;
	t1211 = t965 * t323;
	t1225 = -0.32e2 * t965 * t10 * t652 * t1125 + 0.4e1 * t915 * t16 * t974 + 0.4e1 * t182 * t90 * t949 * ZA + 0.32e2 * t1195 * t968 * t10 - 0.8e1 * t1199 * t993 * t103 + 0.8e1 * t1203 * t118 * n * t919 + 0.8e1 * t1170 * t136 * t955 + 0.64e2 * t1211 * t1044 + 0.16e2 * t1031 * t1032 * t4 + 0.8e1 * t987 * t943 + 0.8e1 * t1199 * t993 * t10 + 0.8e1 * t997 * t1050 * t4;
	t1263 = -0.128e3 * t1015 * t1178 * t1044 + 0.16e2 * t1005 * t988 * t1153 + 0.8e1 * t1058 * t1153 * t919 + 0.16e2 * t1010 * t1120 * xc - 0.8e1 * t954 * t1113 - 0.8e1 * t1203 * t14 * n * t919 - 0.16e2 * t1203 * t14 * t31 * t4 - 0.8e1 * t1203 * t1113 - 0.32e2 * t1195 * t977 * t10 - 0.64e2 * t1211 * t1054 + 0.8e1 * t992 * t967 * t1153 + 0.128e3 * t1015 * t983 * t90 * t4 * t47;

	_C1B = (t127 + t204 + t270 + t329 + t384 + t439 + t501 + t549 + t601 + t647 + t698 + t739 + t782 + t821 + t867 + t909) / (t953 + t1013 + t1057 + t1094 + t1140 + t1182 + t1225 + t1263);
	/****************************************************************************************/
	t1 = n * n;
	t2 = t1 * n;
	t3 = nx * t2;
	t4 = 0.3141592654e1 * ZA;
	t5 = t3 * t4;
	t6 = nx * 0.3141592654e1;
	t7 = t6 * xc;
	t8 = sin(t7);
	t9 = t8 * ZB;
	t10 = n * 0.3141592654e1;
	t11 = exp(t10);
	t12 = t11 * t11;
	t15 = exp(xc * n * 0.3141592654e1);
	t16 = t15 * t15;
	t17 = t16 * t16;
	t18 = t17 * t15;
	t19 = t12 * t18;
	t23 = t1 * t1;
	t24 = nx * t23;
	t25 = ZB * ZB;
	t27 = t18 * t8;
	t28 = 0.3141592654e1 * 0.3141592654e1;
	t29 = xc * xc;
	t30 = t28 * t29;
	t34 = t1 * xc;
	t35 = 0.3141592654e1 * ZB;
	t36 = t34 * t35;
	t37 = cos(t7);
	t38 = ZA * t37;
	t39 = nx * nx;
	t40 = t39 * t12;
	t41 = t16 * t15;
	t43 = t38 * t40 * t41;
	t46 = t25 * n;
	t47 = t46 * 0.3141592654e1;
	t48 = t39 * nx;
	t49 = sin(t6);
	t50 = t48 * t49;
	t51 = t12 * t11;
	t52 = t51 * t17;
	t53 = t50 * t52;
	t56 = t34 * 0.3141592654e1 * t25;
	t57 = t37 * t39;
	t58 = t17 * t41;
	t59 = t12 * t58;
	t60 = t57 * t59;
	t63 = t25 * t18;
	t64 = t57 * n;
	t67 = ZA * ZA;
	t68 = t67 * n;
	t69 = 0.3141592654e1 * t48;
	t70 = t68 * t69;
	t71 = t49 * xc;
	t72 = t17 * t16;
	t73 = t11 * t72;
	t74 = t71 * t73;
	t77 = t1 * t67;
	t78 = t77 * 0.3141592654e1;
	t81 = nx * t25;
	t82 = t81 * t49;
	t83 = t17 * t17;
	t85 = t1 * t83 * t11;
	t87 = nx * ZB;
	t88 = t8 * t2;
	t89 = t87 * t88;
	t90 = 0.3141592654e1 * xc;
	t91 = t12 * t12;
	t92 = ZA * t91;
	t97 = ZB * ZA;
	t98 = t97 * t37;
	t99 = t39 * n;
	t100 = t12 * t41;
	t104 = 0.8e1 * t5 * t9 * t19 + 0.8e1 * t24 * t25 * t27 * t30 + 0.12e2 * t36 * t43 - t47 * t53 - 0.2e1 * t56 * t60 - 0.4e1 * t63 * t64 + 0.6e1 * t70 * t74 + 0.4e1 * t78 * t60 - t82 * t85 + 0.4e1 * t89 * t90 * t92 * t41 + 0.4e1 * t98 * t99 * t100;
	t105 = t67 * t48;
	t106 = t49 * t51;
	t107 = t106 * t72;
	t109 = t1 * 0.3141592654e1;
	t110 = t109 * xc;
	t115 = nx * t67;
	t116 = t115 * t49;
	t117 = t1 * t16;
	t118 = t117 * t11;
	t120 = t2 * t25;
	t121 = t28 * 0.3141592654e1;
	t122 = t121 * t29;
	t123 = t120 * t122;
	t129 = t1 * ZB;
	t130 = t129 * t4;
	t131 = t57 * t100;
	t134 = t12 * t16;
	t136 = t109 * t39;
	t139 = ZB * t18;
	t141 = t39 * t1;
	t142 = t141 * t90;
	t145 = t77 * t90;
	t146 = t91 * t41;
	t147 = t57 * t146;
	t151 = t25 * t39 * t1;
	t152 = t72 * t12;
	t156 = t49 * t2;
	t158 = t83 * t11;
	t162 = -t105 * t107 + 0.8e1 * t110 * t72 * t25 * t39 - t116 * t118 + 0.8e1 * t123 * t53 + 0.8e1 * t5 * t9 * t59 - 0.8e1 * t130 * t131 - 0.8e1 * t134 * t25 * t136 - 0.12e2 * t139 * t38 * t142 - 0.8e1 * t145 * t147 - 0.8e1 * t151 * t90 * t152 - 0.2e1 * t87 * t156 * t4 * t158;
	t164 = t115 * t88;
	t165 = t90 * t19;
	t168 = t25 * t48;
	t169 = t49 * t16;
	t170 = t169 * t11;
	t174 = ZA * n * t69;
	t175 = ZB * t51;
	t176 = t175 * t17;
	t177 = t71 * t176;
	t180 = t1 * t29;
	t181 = t28 * t25;
	t182 = t180 * t181;
	t183 = t50 * t73;
	t186 = ZA * t1;
	t187 = t28 * t48;
	t188 = t186 * t187;
	t189 = ZB * t17;
	t190 = t189 * t11;
	t191 = t71 * t190;
	t194 = t50 * t158;
	t196 = t115 * t156;
	t197 = t90 * t73;
	t201 = t49 * t17 * t11;
	t204 = t88 * t90;
	t207 = t68 * 0.3141592654e1;
	t208 = t17 * t11;
	t209 = t50 * t208;
	t211 = -0.2e1 * t164 * t165 - t168 * t170 + t168 * t107 + 0.8e1 * t174 * t177 + 0.2e1 * t182 * t183 + 0.8e1 * t188 * t191 + t47 * t194 - 0.6e1 * t196 * t197 - t168 * t201 - 0.4e1 * t81 * t18 * t204 - t207 * t209;
	t212 = t2 * 0.3141592654e1;
	t213 = t212 * t52;
	t215 = t81 * t8;
	t216 = t212 * t59;
	t219 = t3 * t90;
	t220 = t25 * t8;
	t221 = t18 * t91;
	t225 = t71 * t52;
	t231 = t16 * t51;
	t232 = t50 * t231;
	t237 = ZA * t12;
	t243 = t67 * t28;
	t244 = t24 * t243;
	t245 = t71 * t231;
	t249 = -t116 * t213 - 0.4e1 * t215 * t216 + 0.2e1 * t219 * t220 * t221 - 0.4e1 * t70 * t225 + 0.4e1 * t98 * t99 * t146 + t47 * t232 - 0.2e1 * t145 * t57 * t221 + 0.4e1 * t89 * t90 * t237 * t41 - t105 * t201 - 0.6e1 * t244 * t245 + t105 * t170;
	t252 = t25 * t37;
	t253 = t252 * t39;
	t255 = n * t15 * t12;
	t258 = t2 * t29;
	t259 = ZB * t28;
	t260 = t258 * t259;
	t263 = t106 * t17;
	t265 = xc * t25;
	t269 = t25 * t49;
	t270 = t269 * t52;
	t273 = t1 * t25;
	t274 = t273 * 0.3141592654e1;
	t275 = t57 * t19;
	t278 = t24 * t30;
	t288 = t1 * t11 * t72;
	t290 = t212 * t208;
	t292 = t2 * xc;
	t296 = 0.2e1 * t253 * t255 + 0.16e2 * t260 * t43 + t105 * t263 - 0.4e1 * t10 * t265 * t53 + 0.4e1 * t219 * t270 - 0.12e2 * t274 * t275 + 0.8e1 * t278 * t270 - 0.2e1 * ZB * n * t69 * t49 * ZA * t158 - t82 * t288 - t116 * t290 + 0.16e2 * t292 * t243 * t275;
	t301 = t50 * t176;
	t304 = t51 * t72;
	t305 = t71 * t304;
	t308 = t25 * t41;
	t311 = ZA * t48;
	t312 = t311 * t49;
	t317 = t91 * t15;
	t318 = t57 * t317;
	t321 = t81 * t88;
	t322 = t90 * t59;
	t325 = t212 * t231;
	t327 = t15 * t12;
	t328 = t57 * t327;
	t331 = t77 * t187;
	t334 = t2 * ZA;
	t335 = t334 * t122;
	t336 = t50 * t190;
	t339 = 0.8e1 * t151 * t90 * t134 + 0.16e2 * t186 * t30 * t301 - 0.2e1 * t70 * t305 + 0.2e1 * t308 * t64 - 0.2e1 * t312 * ZB * t83 * t11 + 0.2e1 * t56 * t318 + 0.2e1 * t321 * t322 - t116 * t325 - 0.4e1 * t274 * t328 + 0.2e1 * t331 * t305 - 0.16e2 * t335 * t336;
	t341 = t169 * t51;
	t344 = t49 * t11 * t72;
	t346 = t77 * t30;
	t347 = t50 * t304;
	t350 = t25 * t51;
	t352 = nx * ZA;
	t353 = t49 * t23;
	t354 = t352 * t353;
	t355 = t28 * xc;
	t362 = t25 * t91;
	t365 = t23 * n;
	t366 = nx * t365;
	t367 = t366 * t122;
	t368 = ZB * t49;
	t369 = ZA * t51;
	t370 = t369 * t17;
	t371 = t368 * t370;
	t374 = t115 * t353;
	t375 = t355 * t73;
	t381 = t105 * t341 - t105 * t344 - 0.2e1 * t346 * t347 - t350 * t50 - 0.8e1 * t354 * t355 * t176 - 0.4e1 * t98 * t99 * t317 - 0.2e1 * t362 * t99 - 0.16e2 * t367 * t371 + 0.6e1 * t374 * t375 - 0.8e1 * t182 * t53 - t82 * t290;
	t382 = t71 * t208;
	t394 = t2 * t67;
	t395 = t394 * t122;
	t398 = t352 * t156;
	t402 = t17 * t12;
	t403 = t39 * ZA;
	t404 = t402 * t403;
	t407 = t269 * t208;
	t411 = t49 * t83 * t11;
	t413 = t46 * t69;
	t419 = -0.4e1 * t331 * t382 + 0.2e1 * t115 * t58 * t204 - 0.2e1 * t145 * t60 + 0.12e2 * t274 * t131 + 0.2e1 * t346 * t232 + 0.8e1 * t395 * t53 - 0.8e1 * t398 * t90 * t176 - 0.64e2 * t260 * t404 + 0.4e1 * t219 * t407 + t168 * t411 - 0.6e1 * t413 * t74 - 0.2e1 * t110 * t308 * t57;
	t424 = t16 * t11;
	t425 = t212 * t424;
	t427 = t258 * t181;
	t430 = t67 * t29;
	t431 = t366 * t430;
	t432 = t121 * t49;
	t433 = t432 * t52;
	t436 = n * t12;
	t437 = t436 * t18;
	t440 = t29 * xc;
	t441 = t440 * t121;
	t442 = t394 * t441;
	t445 = t67 * t37;
	t446 = t445 * t39;
	t448 = n * t18 * t91;
	t453 = t352 * t49;
	t458 = t8 * t23;
	t462 = t81 * t458;
	t463 = t30 * t19;
	t466 = -t47 * t209 + t116 * t425 - 0.8e1 * t427 * t275 + 0.8e1 * t431 * t433 - 0.2e1 * t253 * t437 - 0.8e1 * t442 * t53 - 0.2e1 * t446 * t448 + 0.2e1 * t175 * t312 + 0.6e1 * t453 * t129 * t208 + 0.8e1 * t115 * t18 * t458 * t30 + 0.8e1 * t462 * t463;
	t470 = t436 * t58;
	t475 = t2 * t121 * t440 * t25;
	t485 = t212 * t73;
	t488 = t67 * t72 * t1;
	t490 = t39 * xc;
	t501 = 0.4e1 * t374 * t355 * t52 + 0.2e1 * t446 * t470 - 0.8e1 * t475 * t53 - 0.2e1 * t446 * t437 - 0.4e1 * t36 * t38 * t39 * t15 * t12 - t116 * t485 + 0.8e1 * t488 * 0.3141592654e1 * t12 * t490 - t207 * t183 - 0.2e1 * t182 * t232 - 0.6e1 * t413 * t245 - 0.4e1 * t413 * t382;
	t503 = t115 * t8;
	t510 = t355 * t19;
	t513 = t432 * t208;
	t525 = t38 * t40 * t18;
	t533 = -0.4e1 * t503 * t216 - 0.4e1 * t89 * t90 * t92 * t15 - 0.16e2 * t462 * t510 + 0.8e1 * t431 * t513 - 0.4e1 * t78 * t131 + t47 * t183 - 0.2e1 * t67 * t83 * t99 + 0.4e1 * t331 * t225 + 0.16e2 * t260 * t525 - 0.4e1 * t89 * t90 * t237 * t58 - t207 * t53;
	t536 = t28 * t37;
	t538 = t490 * t100;
	t541 = t334 * t441;
	t547 = t394 * t30;
	t550 = t212 * t19;
	t553 = t366 * t441;
	t556 = n * t17;
	t571 = -0.8e1 * t427 * t131 + 0.16e2 * t394 * t536 * t538 + 0.16e2 * t541 * t336 + 0.2e1 * t453 * t129 * t158 - 0.8e1 * t547 * t147 + 0.4e1 * t503 * t550 - 0.8e1 * t553 * t270 + 0.4e1 * t556 * ZB * t92 * t39 - 0.2e1 * t67 * t91 * t99 - t82 * t425 + 0.4e1 * t78 * t275 + 0.2e1 * t78 * xc * t41 * t57;
	t583 = t90 * t317;
	t594 = t212 * t158;
	t596 = t152 * t67;
	t602 = t67 * t17;
	t607 = 0.8e1 * t367 * t407 - 0.4e1 * t98 * t99 * t59 + 0.16e2 * t260 * t18 * ZA * t57 + 0.2e1 * t321 * t583 - 0.6e1 * t174 * t368 * t52 - 0.4e1 * t89 * t90 * ZA * t15 * t12 + t116 * t594 - 0.8e1 * t596 * t136 - 0.4e1 * t98 * t99 * t327 + 0.2e1 * t602 * t99 + 0.2e1 * t164 * t583;
	t613 = t83 * t25;
	t616 = t81 * t156;
	t627 = t90 * t231;
	t630 = t91 * t16;
	t638 = 0.4e1 * t196 * t90 * t208 - 0.8e1 * t130 * t60 - 0.2e1 * t613 * t99 + 0.6e1 * t616 * t197 - 0.8e1 * t547 * t131 + 0.8e1 * t67 * t18 * t37 * t142 + 0.2e1 * t145 * t328 - 0.6e1 * t196 * t627 + 0.8e1 * t630 * t67 * t142 - 0.8e1 * t547 * t275 + 0.8e1 * t395 * t209;
	t643 = t77 * t355;
	t648 = t115 * t458;
	t651 = t134 * t67;
	t657 = t30 * t304;
	t660 = t30 * t146;
	t665 = t25 * t17;
	t668 = t50 * t424;
	t671 = -0.4e1 * t321 * t90 * t146 - 0.6e1 * t643 * t232 + 0.8e1 * t182 * t209 - 0.16e2 * t648 * t510 + 0.8e1 * t651 * t136 + 0.8e1 * t89 * t4 * t100 - 0.2e1 * t374 * t657 - 0.8e1 * t648 * t660 + 0.8e1 * t130 * t328 + 0.2e1 * t665 * t99 + 0.2e1 * t346 * t668;
	t672 = t90 * t424;
	t676 = t120 * t536;
	t680 = t436 * t41;
	t688 = t366 * t67 * t440;
	t696 = xc * t12;
	t697 = t696 * t18;
	t701 = t252 * t141;
	t702 = t90 * t221;
	t705 = 0.2e1 * t196 * t672 - t47 * t347 + 0.16e2 * t676 * t538 - t116 * t85 - 0.2e1 * t253 * t680 + t207 * t194 + 0.4e1 * t98 * t99 * t19 - 0.8e1 * t688 * t433 + 0.16e2 * t541 * t301 - 0.6e1 * t312 * t190 + 0.4e1 * t352 * t88 * t35 * t697 + 0.2e1 * t701 * t702;
	t712 = t24 * t430;
	t713 = t28 * t49;
	t721 = t1 * t17 * t11;
	t726 = ZB * xc;
	t737 = n * t91;
	t741 = 0.8e1 * t346 * t209 + 0.2e1 * t712 * t713 * t424 + 0.8e1 * t130 * t275 - t47 * t668 + t116 * t721 - 0.8e1 * t688 * t513 + 0.4e1 * t352 * t27 * t212 * t726 + 0.8e1 * t648 * t463 + 0.4e1 * t274 * t60 - 0.4e1 * t374 * t355 * t208 - 0.4e1 * t253 * t737 * t41;
	t745 = t269 * t231;
	t749 = t1 * t28 * t265;
	t757 = t16 * t39;
	t758 = t696 * t757;
	t762 = t69 * t49;
	t772 = t355 * t100;
	t775 = t81 * t353;
	t778 = -0.8e1 * t398 * t90 * t190 - 0.2e1 * t278 * t745 + 0.4e1 * t749 * t53 + 0.32e2 * t394 * t29 * t28 * t17 * t40 - 0.8e1 * t78 * t758 + t350 * n * t762 - 0.6e1 * t87 * t49 * t186 * t52 - 0.8e1 * t553 * t407 - 0.4e1 * t749 * t209 + 0.16e2 * t648 * t772 - 0.6e1 * t775 * t375;
	t790 = t212 * t304;
	t793 = t156 * 0.3141592654e1;
	t795 = t355 * t304;
	t800 = t91 * t39;
	t801 = t800 * n;
	t807 = t2 * t28;
	t808 = t807 * t726;
	t811 = -0.2e1 * t616 * t672 - 0.2e1 * t446 * t680 - 0.2e1 * t78 * xc * t58 * t57 + 0.8e1 * t367 * t270 - t82 * t790 + t115 * t51 * t793 - 0.2e1 * t775 * t795 + 0.8e1 * t123 * t209 + 0.2e1 * t665 * t801 - 0.2e1 * t67 * t41 * t64 - 0.32e2 * t808 * t43;
	t812 = t117 * t51;
	t821 = t24 * t355;
	t827 = t90 * t304;
	t840 = t800 * t41;
	t844 = -t116 * t812 - 0.2e1 * t110 * t25 * t58 * t57 - 0.4e1 * t78 * t328 + t82 * t485 - 0.4e1 * t821 * t407 + 0.4e1 * t196 * t90 * t52 + 0.2e1 * t196 * t827 + t82 * t325 + 0.2e1 * t253 * t448 - 0.32e2 * t402 * t67 * t807 * t490 - t207 * t232 + 0.12e2 * t186 * t90 * ZB * t37 * t840;
	t849 = t1 * t51;
	t850 = t849 * t17;
	t860 = t269 * t424;
	t863 = t273 * t187;
	t874 = 0.16e2 * t462 * t772 - t116 * t850 + 0.16e2 * t553 * t371 + t116 * t288 - 0.12e2 * t97 * t57 * t109 * t697 + t82 * t594 - 0.2e1 * t278 * t860 - 0.2e1 * t863 * t305 - 0.16e2 * t180 * t259 * t311 * t201 - 0.6e1 * t863 * t74 + 0.8e1 * t174 * t191;
	t879 = xc * ZA;
	t888 = t67 * t51;
	t901 = ZA * t17;
	t903 = t368 * t901 * t11;
	t908 = -0.2e1 * t352 * t51 * t156 * t35 + 0.64e2 * t879 * t189 * t807 * t40 + 0.2e1 * t46 * t58 * t37 * t39 - t888 * t50 + t105 * t411 - 0.16e2 * t335 * t301 + 0.8e1 * t152 * t25 * t136 - 0.8e1 * t278 * t407 + 0.2e1 * t712 * t713 * t231 - 0.16e2 * t367 * t903 + 0.2e1 * t145 * t318;
	t923 = t71 * t424;
	t926 = t87 * t458;
	t927 = t28 * ZA;
	t944 = 0.8e1 * t354 * t355 * t190 - 0.8e1 * t110 * t16 * t25 * t800 - 0.2e1 * t374 * t30 * t73 - 0.16e2 * t354 * t30 * t176 - 0.2e1 * t244 * t923 - 0.32e2 * t926 * t927 * t696 * t41 - 0.32e2 * t808 * t525 + 0.6e1 * t749 * t232 - 0.8e1 * t188 * t177 + 0.4e1 * t36 * t58 * ZA * t57 + 0.4e1 * t821 * t270;
	t948 = t90 * t327;
	t961 = t30 * t100;
	t964 = t29 * t49;
	t981 = t106 * t1;
	t983 = -0.2e1 * t219 * t220 * t100 + 0.2e1 * t321 * t948 - 0.16e2 * t189 * ZA * t99 * t12 - 0.2e1 * t369 * n * t69 * t368 + 0.2e1 * t374 * t795 - 0.8e1 * t462 * t961 - 0.8e1 * t244 * t964 * t208 + 0.2e1 * t413 * t923 + 0.4e1 * t36 * t38 * t40 * t58 - 0.2e1 * t87 * t51 * t49 * t1 * ZA + t888 * n * t762 + t115 * t981;
	t1012 = 0.6e1 * t616 * t627 - t82 * t213 + 0.2e1 * t775 * t657 - 0.12e2 * t215 * t550 - 0.6e1 * t145 * t131 + 0.2e1 * t81 * t41 * t204 + 0.6e1 * ZB * t48 * t49 * t370 - 0.4e1 * t70 * t382 + 0.2e1 * t446 * t255 + 0.8e1 * t89 * t4 * t327 - 0.4e1 * t56 * t147;
	t1018 = t212 * t100;
	t1029 = t212 * t327;
	t1040 = 0.6e1 * t70 * t245 + 0.2e1 * t56 * t328 + t207 * t668 + 0.4e1 * t503 * t1018 + 0.2e1 * t253 * t470 - 0.6e1 * t398 * t35 * t208 - 0.8e1 * t331 * t964 * t52 - 0.4e1 * t503 * t1029 + 0.6e1 * t821 * t745 + 0.4e1 * t63 * t37 * t142 + 0.16e2 * t260 * t38 * t840;
	t1068 = t207 * t347 - 0.2e1 * t164 * t702 - 0.2e1 * t331 * t964 * t73 + 0.8e1 * t374 * t30 * t52 + 0.16e2 * t278 * t903 + 0.2e1 * t863 * t923 + 0.6e1 * t445 * t141 * t165 - 0.2e1 * t164 * t90 * t100 + 0.6e1 * t331 * t74 - 0.2e1 * t182 * t668 - 0.2e1 * t115 * t41 * t204;
	t1079 = t58 * t8;
	t1091 = t807 * t29;
	t1092 = t665 * t40;
	t1101 = ZB * t91;
	t1102 = t403 * n;
	t1105 = -0.4e1 * t58 * ZB * ZA * t64 - t82 * t850 + 0.2e1 * t821 * t860 + t81 * t51 * t793 + 0.2e1 * t3 * t25 * t1079 * t90 + t82 * t721 - 0.2e1 * t643 * t668 + 0.16e2 * t926 * t927 * t29 * t91 * t41 + 0.32e2 * t1091 * t1092 - 0.2e1 * t219 * t220 * t19 + 0.4e1 * t139 * ZA * t64 + 0.4e1 * t1101 * t1102;
	t1108 = t849 * t72;
	t1121 = t737 * t15;
	t1124 = t29 * t12;
	t1133 = t116 * t1108 - 0.8e1 * t475 * t209 - 0.32e2 * t807 * xc * t1092 + 0.2e1 * t278 * t269 * t73 + t82 * t812 - 0.6e1 * t56 * t131 + 0.2e1 * t253 * t1121 + 0.16e2 * t926 * t927 * t1124 * t41 + t168 * t263 - 0.2e1 * t616 * t827 + t81 * t981;
	t1134 = t394 * t28;
	t1159 = -0.8e1 * t1134 * t29 * t18 * t57 + t82 * t118 - 0.12e2 * t215 * t1018 + 0.2e1 * t602 * t801 - t168 * t341 + 0.2e1 * t67 * t58 * t64 + t168 * t344 - 0.6e1 * t174 * t368 * t208 + 0.16e2 * t553 * t903 + t116 * t790 - 0.4e1 * t36 * t38 * t800 * t15;
	t1161 = n * t83;
	t1173 = ZB * t12;
	t1196 = 0.4e1 * t1161 * ZB * t39 * ZA - 0.4e1 * t215 * t1029 - 0.8e1 * t488 * 0.3141592654e1 * t39 * xc + 0.32e2 * t821 * ZA * t8 * t1173 * t18 - 0.8e1 * t427 * t147 + 0.6e1 * t701 * t165 - 0.16e2 * t926 * t927 * t1124 * t18 - 0.8e1 * t1091 * t63 * t57 - 0.8e1 * t442 * t209 - 0.8e1 * t462 * t660 - 0.6e1 * t398 * t35 * t52;
	t1228 = 0.2e1 * t413 * t305 - 0.8e1 * t648 * t961 - 0.16e2 * t87 * t27 * t23 * t28 * ZA * t29 + 0.4e1 * t189 * t1102 - 0.4e1 * t87 * t1079 * t212 * t879 + 0.2e1 * t164 * t948 - 0.2e1 * t70 * t923 + 0.2e1 * t164 * t322 + 0.2e1 * t446 * t1121 + 0.2e1 * t863 * t964 * t304 - t82 * t1108 + 0.16e2 * t676 * t490 * t19;
	t1234 = t25 * ZB;
	t1235 = t1234 * t28;
	t1236 = t365 * t91;
	t1240 = ZB * t121;
	t1241 = t1240 * t77;
	t1242 = t39 * t39;
	t1243 = t12 * t1242;
	t1244 = xc * t72;
	t1245 = t1243 * t1244;
	t1248 = t365 * t25;
	t1252 = t243 * n;
	t1257 = t23 * t1;
	t1258 = t1240 * t1257;
	t1259 = t67 * t12;
	t1260 = xc * t16;
	t1268 = t1234 * t121;
	t1269 = t1268 * t23;
	t1272 = t1242 * t91;
	t1280 = t67 * xc;
	t1284 = t28 * t28;
	t1285 = t67 * t1284;
	t1287 = t1285 * t2 * ZB;
	t1288 = t17 * xc;
	t1289 = t1243 * t1288;
	t1292 = 0.2e1 * t1235 * t1236 * t17 + 0.8e1 * t1241 * t1245 + 0.4e1 * t927 * t1248 * t91 - 0.2e1 * t1252 * ZB * t1242 * t91 - 0.8e1 * t1258 * t1259 * t1260 - 0.4e1 * t1235 * t2 * t83 * t39 + 0.16e2 * t1269 * t758 + 0.2e1 * t1252 * t189 * t1272 - 0.2e1 * t1252 * t83 * t1242 * ZB + 0.8e1 * t1258 * t630 * t1280 - 0.32e2 * t1287 * t1289;
	t1293 = t365 * t83;
	t1300 = ZA * t1284;
	t1304 = t17 * t1242 * t25 * t12;
	t1307 = t927 * t2;
	t1311 = t23 * t2;
	t1312 = t1300 * t1311;
	t1316 = t1234 * t1284;
	t1317 = t1316 * t1311;
	t1321 = t1240 * t23;
	t1331 = t1240 * t23 * t67;
	t1332 = t40 * t1244;
	t1338 = t1243 * t1260;
	t1344 = -0.2e1 * t1235 * t1293 - 0.16e2 * t181 * t365 * t901 * t12 - 0.64e2 * t1300 * t258 * t1304 + 0.8e1 * t1307 * t613 * t39 + 0.64e2 * t1312 * t265 * t402 - 0.32e2 * t1317 * t1288 * t12 - 0.16e2 * t1321 * t67 * t39 * t1244 + 0.2e1 * t1235 * n * t1272 * t17 + 0.16e2 * t1331 * t1332 + 0.64e2 * t1300 * t292 * t1304 - 0.8e1 * t1241 * t1338 - 0.2e1 * t243 * t1293 * ZB;
	t1346 = t1316 * t2;
	t1349 = t927 * n;
	t1350 = t25 * t1242;
	t1354 = t1268 * t1257;
	t1366 = t1268 * t1;
	t1370 = t29 * t17;
	t1371 = t1243 * t1370;
	t1386 = -0.32e2 * t1346 * t1289 + 0.4e1 * t1349 * t1350 * t91 + 0.8e1 * t1354 * t1260 * t12 - 0.16e2 * t181 * n * t901 * t1243 - 0.4e1 * t1235 * t2 * t91 * t39 + 0.8e1 * t1366 * t152 * t1242 + 0.32e2 * t1287 * t1371 + 0.8e1 * t1258 * t1280 * t152 - 0.8e1 * t1354 * t1260 * t91 + 0.128e3 * t1300 * t365 * xc * t1092 + 0.8e1 * t1366 * t1338;
	t1387 = t1257 * t12;
	t1391 = t1240 * t1;
	t1399 = t1272 * t1260;
	t1412 = t1285 * t1311;
	t1427 = -0.8e1 * t1268 * t1387 * t16 - 0.8e1 * t1391 * t67 * t1242 * t1244 - 0.4e1 * t1134 * t1101 * t39 + 0.8e1 * t1241 * t1399 - 0.8e1 * t1258 * t596 + 0.4e1 * t927 * t1293 * t25 - 0.16e2 * t1331 * t758 + 0.8e1 * t1307 * t665 * t39 + 0.32e2 * t1412 * t1370 * t1173 + 0.8e1 * t1307 * t665 * t800 + 0.8e1 * t1391 * t1259 * t1242 * t16 - 0.8e1 * t1391 * t1259 * t1242 * t72;
	t1456 = t365 * ZB;
	t1468 = 0.4e1 * t927 * t1248 * t17 - 0.2e1 * t1235 * n * t1242 * t91 + 0.8e1 * t1366 * t1244 * t1242 - 0.16e2 * t1269 * t134 * t39 + 0.8e1 * t1268 * t1257 * t72 * xc + 0.16e2 * t1321 * t1259 * t757 + 0.32e2 * t1317 * t1370 * t12 + 0.4e1 * t1349 * t613 * t1242 + 0.2e1 * t243 * t1456 * t17 - 0.64e2 * t1285 * t365 * t12 * t189 * t490 - 0.8e1 * t1354 * t152 * xc;
	t1472 = t1316 * t365;
	t1474 = t1124 * t39 * t17;
	t1478 = t17 * t91;
	t1504 = t72 * t39;
	t1511 = 0.4e1 * t1134 * t189 * t800 + 0.64e2 * t1472 * t1474 + 0.4e1 * t1235 * t2 * t1478 * t39 + 0.4e1 * t1349 * t665 * t1242 - 0.8e1 * t1258 * t1280 * t72 + 0.2e1 * t1252 * t189 * t1242 + 0.2e1 * t243 * t365 * t189 * t91 + 0.4e1 * t927 * t365 * t1478 * t25 - 0.128e3 * t1300 * t1248 * t1474 - 0.2e1 * t1235 * t1236 + 0.16e2 * t1269 * t1504 * xc + 0.2e1 * t1235 * t365 * t17;
	t1545 = -0.2e1 * t1235 * t1161 * t1242 + 0.4e1 * t1349 * t1350 * t1478 - 0.8e1 * t1366 * t1245 + 0.2e1 * t1235 * t556 * t1242 - 0.32e2 * t1412 * t402 * t726 - 0.8e1 * t1366 * t1399 + 0.8e1 * t1258 * t651 - 0.2e1 * t243 * t1456 * t91 + 0.8e1 * t1268 * t1387 * t72 - 0.16e2 * t1269 * t1332 + 0.4e1 * t1134 * t189 * t39 + 0.16e2 * t1269 * t152 * t39;
	t1564 = t1260 * t800;
	t1583 = 0.64e2 * t1285 * t1456 * t1474 - 0.64e2 * t1472 * t1288 * t40 - 0.8e1 * t1366 * t134 * t1242 + 0.8e1 * t1307 * t362 * t39 + 0.4e1 * t1235 * t2 * t17 * t39 + 0.32e2 * t1346 * t1371 - 0.16e2 * t1269 * t1564 - 0.16e2 * t1321 * t1259 * t1504 + 0.16e2 * t1331 * t1564 - 0.64e2 * t1312 * t29 * t25 * t402 - 0.4e1 * t1134 * t83 * t39 * ZB - 0.32e2 * t181 * t2 * t404;

	_C2B = (t1133 + t1196 + t1068 + t811 + t466 + t1012 + t381 + t162 + t249 + t533 + t844 + t104 + t1159 + t571 + t211 + t874 + t607 + t339 + t296 + t638 + t908 + t671 + t419 + t983 + t705 + t1105 + t501 + t778 + t1040 + t1228 + t741 + t944) / (t1292 + t1344 + t1386 + t1427 + t1468 + t1511 + t1545 + t1583);
	/****************************************************************************************/
	t1 = n * n;
	t2 = t1 * n;
	t3 = t2 * nx;
	t4 = nx * 0.3141592654e1;
	t5 = t4 * xc;
	t6 = sin(t5);
	t7 = 0.3141592654e1 * 0.3141592654e1;
	t9 = t3 * t6 * t7;
	t10 = xc * xc;
	t11 = ZA * ZA;
	t12 = t10 * t11;
	t13 = n * 0.3141592654e1;
	t14 = exp(t13);
	t15 = t14 * t14;
	t16 = xc * n;
	t18 = exp(t16 * 0.3141592654e1);
	t19 = t18 * t18;
	t20 = t19 * t18;
	t21 = t15 * t20;
	t22 = t12 * t21;
	t25 = nx * t6;
	t26 = t1 * 0.3141592654e1;
	t27 = t25 * t26;
	t28 = ZA * ZB;
	t29 = t18 * t15;
	t30 = t28 * t29;
	t33 = t25 * n;
	t34 = t11 * t15;
	t35 = t19 * t19;
	t36 = t35 * t18;
	t40 = t25 * t1;
	t41 = 0.3141592654e1 * t11;
	t42 = t15 * t36;
	t43 = t41 * t42;
	t46 = nx * nx;
	t47 = t1 * t46;
	t48 = t47 * t11;
	t49 = t7 * xc;
	t50 = t35 * t15;
	t51 = t49 * t50;
	t55 = sin(t4);
	t56 = t46 * nx * t55;
	t58 = t56 * n * t7;
	t59 = ZB * ZB;
	t60 = t10 * t59;
	t61 = t15 * t14;
	t62 = t19 * t61;
	t63 = t60 * t62;
	t66 = t19 * t14;
	t67 = t60 * t66;
	t70 = t28 * t42;
	t73 = cos(t5);
	t74 = t47 * t73;
	t75 = t7 * t11;
	t77 = t75 * t10 * t36;
	t80 = t73 * t46;
	t81 = t80 * n;
	t82 = 0.3141592654e1 * t59;
	t83 = t82 * t42;
	t87 = xc * t11;
	t88 = t87 * t62;
	t91 = n * nx;
	t92 = t55 * t61;
	t96 = nx * t55;
	t98 = t96 * t2 * t7;
	t101 = xc * t59;
	t102 = t101 * t62;
	t108 = t1 * t1;
	t109 = t108 * t7;
	t111 = t59 * t35;
	t112 = t111 * t15;
	t115 = t35 * t20;
	t123 = t1 * nx * t55;
	t124 = t61 * t35;
	t127 = t35 * t19;
	t128 = t61 * t127;
	t129 = t60 * t128;
	t132 = t56 * t16;
	t133 = t7 * t59;
	t134 = t133 * t124;
	t137 = 0.6e1 * t58 * t88 - 0.2e1 * t91 * t92 * t11 + 0.2e1 * t98 * t63 - 0.6e1 * t58 * t102 - 0.2e1 * t91 * t92 * t59 - 0.16e2 * t109 * xc * t112 - 0.2e1 * t91 * t6 * t115 * t59 + 0.12e2 * t40 * t83 + t123 * t41 * t124 - 0.2e1 * t58 * t129 + 0.4e1 * t132 * t134;
	t139 = t56 * 0.3141592654e1;
	t140 = t111 * t14;
	t144 = t49 * t124;
	t147 = t91 * t55;
	t148 = t61 * ZA;
	t154 = ZA * t115 * xc * ZB;
	t157 = t7 * 0.3141592654e1;
	t159 = t96 * t108 * t157;
	t160 = t10 * xc;
	t161 = t160 * t59;
	t162 = t35 * t14;
	t163 = t161 * t162;
	t166 = t28 * t162;
	t169 = t80 * t13;
	t170 = t101 * t42;
	t173 = t2 * t11;
	t174 = t96 * t173;
	t175 = t7 * t10;
	t179 = t59 * t15;
	t184 = t15 * t15;
	t193 = t139 * t140 + 0.4e1 * t56 * n * t11 * t144 + 0.4e1 * t147 * t148 * ZB + 0.4e1 * t27 * t154 + 0.8e1 * t159 * t163 - 0.12e2 * t147 * t166 + 0.2e1 * t169 * t170 - 0.16e2 * t174 * t175 * t124 + 0.2e1 * t33 * t179 * t20 - 0.2e1 * t33 * t11 * t36 * t184 + 0.2e1 * t56 * t61 * 0.3141592654e1 * ZA * ZB;
	t194 = t173 * 0.3141592654e1;
	t195 = xc * t15;
	t196 = t195 * t19;
	t202 = t15 * t115;
	t203 = t28 * t202;
	t206 = t96 * t26;
	t207 = t14 * t127;
	t208 = t101 * t207;
	t211 = t12 * t128;
	t218 = t11 * t61;
	t219 = t218 * t35;
	t221 = t108 * ZA;
	t223 = t7 * ZB;
	t224 = t223 * t50;
	t227 = ZA * xc;
	t228 = ZB * t15;
	t229 = t228 * t36;
	t230 = t227 * t229;
	t233 = t87 * t207;
	t236 = t6 * t11;
	t240 = -0.4e1 * t194 * t196 + 0.4e1 * t194 * t195 * t127 + 0.4e1 * t33 * t203 - 0.12e2 * t206 * t208 + 0.2e1 * t58 * t211 - 0.16e2 * t47 * t10 * t133 * t50 + t139 * t219 - 0.32e2 * t221 * t10 * t224 - 0.4e1 * t169 * t230 - 0.6e1 * t98 * t233 + 0.2e1 * t91 * t236 * t20;
	t244 = t227 * t228 * t20;
	t252 = t184 * t18;
	t253 = t101 * t252;
	t256 = t35 * t35;
	t257 = t256 * t14;
	t258 = t28 * t257;
	t261 = t108 * t11;
	t263 = t7 * t35;
	t268 = ZB * t61 * t35;
	t273 = t96 * t108 * t160;
	t274 = t157 * ZB;
	t276 = t274 * t148 * t35;
	t279 = t101 * t21;
	t282 = 0.3141592654e1 * xc;
	t283 = t59 * t36;
	t284 = t282 * t283;
	t289 = 0.4e1 * t169 * t244 - 0.4e1 * t132 * t133 * t162 - 0.2e1 * t147 * t140 - 0.2e1 * t27 * t253 + 0.2e1 * t139 * t258 + 0.16e2 * t261 * t10 * t263 * t15 - 0.16e2 * t206 * t227 * t268 - 0.16e2 * t273 * t276 - 0.6e1 * t27 * t279 - 0.4e1 * t40 * t284 - 0.32e2 * t9 * t230;
	t290 = t1 * t11;
	t291 = t96 * t290;
	t297 = t59 * t61;
	t298 = t297 * t127;
	t300 = ZB * t36;
	t301 = t227 * t300;
	t304 = t1 * t59;
	t305 = t184 * t35;
	t310 = t46 * ZB;
	t311 = t184 * ZA;
	t312 = t310 * t311;
	t314 = t60 * t21;
	t317 = t1 * ZA;
	t318 = ZB * t35;
	t321 = t1 * t256;
	t324 = t96 * t261;
	t325 = t10 * t157;
	t326 = t325 * t124;
	t329 = -0.4e1 * t291 * t282 * t128 + t123 * t82 * t62 - t139 * t298 + 0.12e2 * t27 * t301 + t304 * t305 - 0.2e1 * t58 * t12 * t66 - 0.2e1 * t312 + 0.8e1 * t9 * t314 + 0.2e1 * t317 * t318 + 0.2e1 * t321 * t28 - 0.8e1 * t324 * t326;
	t331 = t28 * t124;
	t334 = 0.3141592654e1 * t15;
	t335 = t334 * t127;
	t338 = t35 * ZA;
	t341 = t46 * t256;
	t344 = t46 * t11;
	t346 = t46 * t59;
	t348 = t297 * t35;
	t351 = ZA * t10;
	t352 = t351 * t300;
	t355 = t1 * ZB;
	t362 = 0.12e2 * t147 * t331 - 0.4e1 * t173 * t335 - 0.2e1 * t310 * t338 - 0.2e1 * t341 * t28 - t344 * t305 - t346 * t305 + 0.2e1 * t147 * t348 + 0.16e2 * t9 * t352 + 0.2e1 * t355 * t311 + t290 * t305 + 0.2e1 * t33 * t34 * t20;
	t363 = t36 * t184;
	t364 = t87 * t363;
	t368 = t47 * t73 * t7;
	t373 = t160 * t157;
	t374 = t373 * t124;
	t377 = t311 * t35;
	t380 = t12 * t62;
	t386 = ZB * t10 * ZA * t15 * t20;
	t389 = t87 * t66;
	t393 = t56 * t1 * t10;
	t401 = 0.2e1 * t27 * t364 - 0.16e2 * t368 * t279 - t123 * t41 * t257 + 0.8e1 * t324 * t374 + 0.2e1 * t355 * t377 - 0.2e1 * t98 * t380 - 0.16e2 * t9 * t386 + 0.2e1 * t58 * t389 + 0.16e2 * t393 * t276 + t123 * t82 * t162 - 0.2e1 * t33 * t179 * t36;
	t412 = t11 * t14 * t127;
	t416 = t11 * t19;
	t417 = t416 * t61;
	t421 = t96 * t2 * ZA;
	t426 = t56 * n * ZA;
	t427 = t318 * t14;
	t428 = t49 * t427;
	t431 = t82 * t29;
	t434 = t87 * t21;
	t442 = 0.2e1 * t33 * t11 * t184 * t18 + 0.4e1 * t81 * t284 - t139 * t412 + 0.2e1 * t147 * t219 - 0.2e1 * t147 * t417 + 0.32e2 * t421 * t175 * t268 + 0.8e1 * t426 * t428 + 0.4e1 * t81 * t431 - 0.2e1 * t169 * t434 - 0.2e1 * t98 * t129 - 0.32e2 * t47 * t28 * t51;
	t443 = t184 * t20;
	t447 = t61 * 0.3141592654e1;
	t448 = t447 * t11;
	t450 = t49 * t268;
	t453 = t60 * t42;
	t456 = t41 * t202;
	t463 = t101 * t443;
	t469 = t41 * xc * t20;
	t474 = -0.8e1 * t27 * t87 * t443 - t56 * t448 - 0.8e1 * t426 * t450 + 0.8e1 * t368 * t453 + 0.4e1 * t40 * t456 + 0.4e1 * t40 * t431 - 0.4e1 * t81 * t456 - 0.4e1 * t27 * t463 + 0.6e1 * t139 * t331 + 0.2e1 * t40 * t469 - 0.16e2 * t9 * t434;
	t482 = t108 * t10;
	t492 = n * t46;
	t493 = t492 * t11;
	t495 = t282 * t19 * t184;
	t498 = t56 * t290;
	t499 = t325 * t162;
	t502 = t416 * t14;
	t504 = t60 * t207;
	t507 = -t123 * t82 * t257 - 0.4e1 * t169 * t301 + t123 * t41 * t162 + 0.16e2 * t482 * t7 * t112 - 0.12e2 * t206 * t102 - t123 * t82 * t66 - 0.4e1 * t147 * t258 - 0.4e1 * t493 * t495 - 0.8e1 * t498 * t499 + t139 * t502 - 0.2e1 * t98 * t504;
	t508 = t101 * t162;
	t512 = t41 * t115 * xc;
	t515 = t87 * t42;
	t520 = ZB * t184;
	t522 = t227 * t520 * t18;
	t525 = t492 * t59;
	t528 = t6 * t59;
	t532 = t520 * t20;
	t533 = t351 * t532;
	t539 = t447 * t59;
	t544 = 0.8e1 * t206 * t508 - 0.2e1 * t40 * t512 - 0.16e2 * t368 * t515 + 0.12e2 * t206 * t88 + 0.4e1 * t27 * t522 + 0.4e1 * t525 * t495 - 0.4e1 * t91 * t528 * t36 - 0.16e2 * t368 * t533 - 0.16e2 * t206 * t227 * t427 - t56 * t539 - 0.2e1 * t132 * t133 * t66;
	t551 = t87 * t162;
	t554 = t351 * t229;
	t560 = t59 * t19;
	t561 = t560 * t14;
	t564 = t101 * t202;
	t567 = t87 * t252;
	t573 = t227 * t228 * t115;
	t578 = 0.4e1 * t33 * t70 + 0.4e1 * t493 * t335 - 0.4e1 * t58 * t551 + 0.16e2 * t9 * t554 - 0.4e1 * t33 * t28 * t252 + 0.2e1 * t147 * t561 + 0.2e1 * t169 * t564 - 0.2e1 * t27 * t567 - 0.8e1 * t324 * t499 - 0.4e1 * t169 * t573 + 0.12e2 * t27 * t244;
	t579 = t82 * t202;
	t591 = t282 * t115 * t59;
	t598 = t101 * t66;
	t606 = -0.4e1 * t81 * t579 - 0.2e1 * t169 * t567 - 0.6e1 * t27 * t170 + 0.8e1 * t169 * t203 + 0.2e1 * t98 * t67 + 0.2e1 * t81 * t591 + 0.32e2 * t368 * t244 - 0.2e1 * t27 * t564 + 0.4e1 * t206 * t598 + 0.16e2 * t9 * t170 + 0.2e1 * t33 * t283 * t184;
	t608 = t373 * t162;
	t611 = t59 * t184;
	t617 = t101 * t29;
	t624 = t227 * ZB * t18 * t15;
	t629 = t157 * t59;
	t630 = t629 * t124;
	t633 = t3 * t6;
	t634 = t175 * t283;
	t644 = 0.8e1 * t498 * t608 + 0.2e1 * t33 * t611 * t18 - 0.4e1 * t206 * t389 - 0.2e1 * t27 * t617 - 0.4e1 * t169 * t154 + 0.4e1 * t27 * t624 + 0.12e2 * t27 * t230 - 0.8e1 * t393 * t630 - 0.8e1 * t633 * t634 + 0.16e2 * t47 * t7 * t101 * t50 + 0.2e1 * t123 * t447 * t28;
	t645 = t41 * t29;
	t648 = t2 * 0.3141592654e1;
	t649 = t648 * xc;
	t650 = t560 * t184;
	t656 = t56 * t1 * t157;
	t659 = t87 * t128;
	t662 = t96 * t482;
	t663 = t629 * t162;
	t671 = t161 * t124;
	t674 = t218 * t127;
	t679 = 0.4e1 * t81 * t645 - 0.4e1 * t649 * t650 - 0.8e1 * t169 * t70 + 0.8e1 * t656 * t163 - 0.2e1 * t98 * t659 - 0.8e1 * t662 * t663 - 0.32e2 * t421 * t175 * t427 - 0.2e1 * t147 * t502 + 0.8e1 * t656 * t671 + 0.2e1 * t147 * t674 - 0.16e2 * t368 * t386;
	t714 = t334 * t19;
	t719 = t12 * t42;
	t722 = t304 * t35 - t346 * t35 + t341 * t59 - t344 * t35 + t344 * t256 + t346 * t184 - 0.16e2 * t368 * t554 - 0.16e2 * t48 * t175 * t50 + 0.4e1 * t525 * t714 - 0.2e1 * t58 * t659 + 0.8e1 * t368 * t719;
	t730 = xc * t19;
	t735 = t59 * t256 * t14;
	t752 = 0.4e1 * t173 * t714 - 0.6e1 * t27 * t515 - 0.16e2 * t9 * t279 + 0.4e1 * t194 * t730 * t184 - t139 * t735 - 0.4e1 * t492 * t127 * t82 * xc - 0.4e1 * t98 * t508 - t123 * t41 * t207 - 0.2e1 * t147 * t298 + 0.8e1 * t368 * t314 + 0.6e1 * t132 * t133 * t207;
	t755 = t28 * t21;
	t759 = t274 * t338 * t14;
	t767 = t11 * t35;
	t768 = t767 * t14;
	t778 = t560 * t61;
	t781 = -0.2e1 * t58 * t504 - 0.8e1 * t27 * t755 + 0.16e2 * t662 * t759 + 0.12e2 * t291 * t282 * t207 - 0.6e1 * t27 * t434 + t139 * t768 - 0.8e1 * t498 * t326 + 0.4e1 * t33 * t611 * t20 + 0.2e1 * t81 * t512 - t139 * t561 + 0.2e1 * t147 * t778;
	t786 = t12 * t443;
	t790 = t282 * t59 * t20;
	t796 = t59 * t14 * t127;
	t806 = t41 * t21;
	t811 = -0.8e1 * t393 * t663 + 0.8e1 * t368 * t786 + 0.2e1 * t81 * t790 + 0.4e1 * t169 * t624 + t139 * t796 + 0.2e1 * t206 * t258 - 0.2e1 * t40 * t591 - 0.8e1 * t662 * t630 - 0.4e1 * t33 * t30 - 0.4e1 * t40 * t806 + 0.8e1 * t9 * t786;
	t819 = t282 * t15 * t127;
	t822 = t101 * t363;
	t830 = t11 * t256 * t14;
	t835 = t227 * t532;
	t842 = 0.2e1 * t33 * t11 * t18 * t15 + t123 * t41 * t66 - 0.4e1 * t493 * t819 - 0.2e1 * t27 * t822 - 0.16e2 * t368 * t170 - 0.4e1 * t169 * t463 - t139 * t830 - 0.4e1 * t649 * t179 * t127 + 0.12e2 * t27 * t835 - 0.16e2 * t368 * t434 - 0.2e1 * t40 * t790;
	t845 = t87 * t202;
	t854 = t338 * t15;
	t859 = t12 * t207;
	t868 = t139 * t348 - 0.2e1 * t27 * t845 + 0.8e1 * t169 * t755 - 0.2e1 * t58 * t380 + 0.6e1 * t206 * t331 + 0.8e1 * t310 * t854 - 0.2e1 * t169 * t822 + 0.2e1 * t98 * t859 + 0.8e1 * t159 * t671 + 0.8e1 * t74 * t634 - 0.2e1 * t169 * t253;
	t880 = t60 * t443;
	t891 = t101 * t128;
	t894 = -t123 * t539 - 0.2e1 * t147 * t796 + 0.32e2 * t368 * t230 + t139 * t674 - 0.16e2 * t98 * t60 * t124 + 0.32e2 * t9 * t244 + 0.8e1 * t368 * t880 - 0.8e1 * t40 * t41 * xc * t36 - t123 * t82 * t128 - 0.6e1 * t58 * t233 + 0.2e1 * t58 * t891;
	t903 = t179 * t19;
	t920 = t56 * t1 * t160;
	t925 = -0.2e1 * t174 * t175 * t66 - 0.4e1 * t493 * t714 + 0.4e1 * t649 * t903 - 0.4e1 * t81 * t43 + t123 * t82 * t207 + 0.4e1 * t206 * t891 - 0.16e2 * t273 * t759 - 0.8e1 * t27 * t203 + 0.32e2 * t221 * ZB * t51 - 0.16e2 * t920 * t759 - 0.8e1 * t9 * t453;
	t932 = t87 * t29;
	t945 = t82 * t21;
	t953 = -0.16e2 * t920 * t276 - 0.8e1 * t169 * t30 - 0.8e1 * t633 * t77 - 0.2e1 * t27 * t932 - 0.4e1 * t174 * t49 * t162 + 0.8e1 * t206 * t87 * t124 - 0.2e1 * t147 * t768 + 0.4e1 * t169 * t522 - 0.12e2 * t81 * t945 + 0.4e1 * t33 * t28 * t115 + 0.4e1 * t525 * t819;
	t971 = t282 * t127;
	t978 = -0.6e1 * t98 * t102 + 0.2e1 * t169 * t515 - 0.2e1 * t310 * t377 + 0.2e1 * t147 * t830 + 0.8e1 * t368 * t22 - 0.2e1 * t169 * t617 + 0.16e2 * t662 * t276 - 0.8e1 * t355 * t854 + 0.4e1 * t493 * t971 - 0.16e2 * t9 * t533 - 0.2e1 * t169 * t279;
	t997 = xc * t127;
	t998 = t997 * t59;
	t1003 = 0.4e1 * t40 * t579 + 0.2e1 * t169 * t845 + 0.16e2 * t9 * t515 + 0.8e1 * t206 * t551 + t123 * t41 * t128 + 0.16e2 * t98 * t60 * t162 + 0.2e1 * t169 * t364 - 0.2e1 * t169 * t932 + t139 * t778 + 0.4e1 * t648 * t998 + 0.2e1 * t147 * t412;
	t1006 = t2 * t59;
	t1017 = xc * t35;
	t1033 = 0.4e1 * t1006 * t335 + 0.4e1 * t81 * t806 - 0.2e1 * t33 * t34 * t115 + 0.8e1 * t498 * t374 - 0.16e2 * t261 * t7 * t1017 * t15 + 0.8e1 * t206 * t101 * t124 - t123 * t448 + 0.2e1 * t147 * t735 + 0.6e1 * t98 * t208 + 0.6e1 * t98 * t88 - 0.4e1 * t33 * t755;
	t1055 = -0.4e1 * t173 * t971 + 0.2e1 * t98 * t891 + 0.8e1 * t9 * t880 + 0.4e1 * t169 * t835 - t304 * t184 + t344 * t184 - t123 * t41 * t62 - 0.2e1 * t98 * t598 + 0.2e1 * t58 * t859 + 0.32e2 * t47 * t351 * t224 + 0.2e1 * t98 * t389;
	t1070 = t15 * t19;
	t1089 = -0.16e2 * t368 * t352 - 0.8e1 * t9 * t719 + 0.4e1 * t96 * t2 * xc * t134 - 0.2e1 * t91 * t236 * t115 + 0.4e1 * t27 * t573 + 0.4e1 * t493 * t282 * t1070 + 0.2e1 * t33 * t59 * t18 * t15 + 0.12e2 * t40 * t945 - 0.4e1 * t492 * xc * t82 * t1070 - 0.2e1 * t91 * t528 * t20 + 0.8e1 * t324 * t608;
	t1113 = t123 * t82 * t124 + 0.8e1 * t421 * t428 - t139 * t417 + 0.4e1 * t40 * t645 + 0.16e2 * t393 * t759 - 0.2e1 * t33 * t179 * t115 - 0.4e1 * t525 * t335 + 0.4e1 * t33 * t28 * t36 - 0.4e1 * t1006 * t714 + 0.6e1 * t206 * t166 - 0.8e1 * t421 * t450;
	t1119 = t321 * t46;
	t1122 = t157 * t11;
	t1123 = t1122 * t2;
	t1124 = t184 * t46;
	t1128 = t108 * n;
	t1132 = t7 * t7;
	t1133 = t1132 * t11;
	t1134 = t1133 * t108;
	t1135 = t15 * t46;
	t1139 = t7 * ZA;
	t1140 = t1139 * ZB;
	t1141 = t1 * t35;
	t1145 = t629 * t2;
	t1146 = t1135 * t730;
	t1149 = t157 * t1128;
	t1150 = t1149 * xc;
	t1153 = t46 * xc;
	t1154 = t1153 * t127;
	t1158 = t184 * t1 * t46;
	t1161 = t46 * t46;
	t1162 = t35 * t1161;
	t1166 = t7 * t1;
	t1170 = -0.4e1 * t133 * t1119 + 0.16e2 * t1123 * t1124 * t730 - 0.8e1 * t1122 * t1128 * t196 - 0.64e2 * t1134 * t1135 * t1017 - 0.32e2 * t1140 * t1141 * t1135 + 0.16e2 * t1145 * t1146 - 0.8e1 * t1150 * t650 - 0.16e2 * t1123 * t1154 - 0.4e1 * t133 * t1158 - 0.16e2 * t1140 * t1162 * t15 + 0.8e1 * t1166 * t35 * t312;
	t1171 = t1161 * t184;
	t1175 = t1122 * n;
	t1176 = t15 * t1161;
	t1180 = t1132 * ZA;
	t1181 = t1180 * t355;
	t1182 = t1176 * t1017;
	t1185 = t1161 * xc;
	t1189 = t1133 * t1;
	t1192 = t108 * t1;
	t1193 = t1132 * t1192;
	t1195 = t10 * t35;
	t1199 = t157 * t15;
	t1203 = t1141 * t46;
	t1211 = t184 * t108;
	t1218 = 0.2e1 * t133 * t1171 * t35 + 0.8e1 * t1175 * t1176 * t997 + 0.64e2 * t1181 * t1182 - 0.8e1 * t1175 * t1185 * t127 - 0.32e2 * t1189 * t1182 - 0.64e2 * t1193 * ZA * t1195 * t228 + 0.8e1 * t1199 * t416 * t1128 + 0.8e1 * t1140 * t1203 - 0.4e1 * t75 * t1158 - 0.8e1 * t1199 * t560 * t1128 - 0.2e1 * t133 * t1211 - 0.8e1 * t1199 * t127 * t11 * t1128;
	t1221 = t256 * t1161;
	t1224 = t35 * t108;
	t1233 = t7 * t256;
	t1236 = -t75 * t1211 - t75 * t1221 - t133 * t1221 + t75 * t1224 - t75 * t1171 - t133 * t1171 + t133 * t1224 + t75 * t1162 - t75 * t108 * t256 + t133 * t1162 - t1233 * t59 * t108;
	t1240 = t1135 * t1195;
	t1252 = t629 * t127;
	t1263 = t1171 * ZA;
	t1280 = -0.128e3 * t1180 * ZB * t108 * t1240 + 0.32e2 * t1193 * t10 * t112 + 0.4e1 * t133 * t1203 + 0.4e1 * t109 * t256 * ZA * ZB - 0.8e1 * t1252 * n * t15 * t1185 + 0.8e1 * t1175 * t1171 * t730 - 0.8e1 * t1175 * t1176 * t127 + 0.4e1 * t223 * t1263 - 0.8e1 * t1175 * t1176 * t730 + 0.8e1 * t1166 * ZA * t341 * ZB + 0.64e2 * t1134 * t1240 + 0.8e1 * t1122 * xc * t1128 * t127 * t15;
	t1283 = t1199 * t19;
	t1287 = t1199 * t127;
	t1289 = t59 * n * t1161;
	t1293 = t157 * n * xc;
	t1304 = t1132 * t108;
	t1310 = t263 * ZB;
	t1316 = t2 * t15;
	t1323 = -0.16e2 * t1283 * t1006 * t46 + 0.8e1 * t1287 * t1289 + 0.8e1 * t1293 * t127 * t1161 * t59 + 0.16e2 * t1123 * t1135 * t19 + 0.8e1 * t1293 * t560 * t1176 + 0.64e2 * t1304 * t59 * t1240 + 0.4e1 * t75 * t1203 + 0.4e1 * t1310 * t1263 + 0.4e1 * t223 * t338 * t108 - 0.16e2 * t1252 * t1316 * t1153 - 0.16e2 * t1310 * t221 * t15;
	t1330 = t1132 * t15;
	t1336 = t1132 * t1;
	t1338 = t1162 * t179;
	t1370 = 0.8e1 * t1175 * t1176 * t19 + 0.4e1 * t1139 * t318 * t1161 + 0.128e3 * t1330 * t318 * t108 * t46 * t227 - 0.32e2 * t1336 * xc * t1338 + 0.4e1 * t1233 * ZA * t1161 * ZB - 0.8e1 * t1287 * t59 * t1128 * xc + 0.2e1 * t75 * t305 * t108 + 0.8e1 * t1199 * t127 * t59 * t1128 - 0.8e1 * t1283 * t1289 - 0.8e1 * t1293 * t560 * t1171 + 0.4e1 * t133 * t35 * t1158 + 0.8e1 * t157 * t184 * t19 * t11 * t1128 * xc;
	t1376 = t7 * t184;
	t1380 = t1176 * t1195;
	t1393 = t1330 * t35;
	t1411 = 0.16e2 * t1145 * t1154 + 0.8e1 * t1149 * t998 + 0.4e1 * t1376 * t35 * t48 + 0.32e2 * t1189 * t1380 + 0.32e2 * t1193 * t11 * t1195 * t15 - 0.64e2 * t1304 * xc * t111 * t1135 - 0.16e2 * t1123 * t1146 + 0.64e2 * t1393 * t28 * t1192 * xc - 0.16e2 * t1123 * t1135 * t127 - 0.8e1 * t1122 * xc * t1128 * t127 - 0.32e2 * t1193 * xc * t112 + 0.16e2 * t1252 * t1316 * t46;
	t1450 = 0.2e1 * t1376 * t767 * t1161 + 0.2e1 * t1376 * t111 * t108 + 0.4e1 * t223 * t311 * t108 + 0.4e1 * t109 * t35 * t520 * ZA + 0.16e2 * t1123 * t1135 * t997 - 0.64e2 * t1181 * t1380 + 0.8e1 * t1150 * t903 - 0.32e2 * t1393 * t11 * t1192 * xc - 0.16e2 * t157 * t2 * xc * t560 * t1124 + 0.8e1 * t223 * t184 * t317 * t46 + 0.32e2 * t1336 * t10 * t1338 - 0.4e1 * t75 * t1119;
	_C3B = (t606 + t722 + t1089 + t781 + 0.16e2 * t48 * t51 + t978 + t868 + t507 - t304 * t256 + 0.8e1 * t9 * t22 + t752 + 0.4e1 * t174 * t144 - 0.2e1 * t81 * t469 + 0.6e1 * t139 * t166 + t362 + 0.2e1 * t98 * t211 + t925 + t137 - t290 * t184 + 0.12e2 * t81 * t83 + t842 + 0.8e1 * t74 * t77 + 0.16e2 * t98 * t12 * t162 - 0.4e1 * t33 * t28 * t443 - 0.8e1 * t27 * t70 - 0.2e1 * t33 * t34 * t36 - 0.8e1 * t27 * t30 + 0.2e1 * t58 * t67 - 0.4e1 * t40 * t43 + 0.2e1 * t58 * t63 + t1033 - t290 * t256 + t290 * t35 + t193 + t1113 + t578 + t442 + t474 + t544 + t329 + t679 + t401 + t953 + t811 + t644 + t894 + t289 + t240 + t1055 + t1003) / (t1170 + t1218 + 0.2e1 * t1236 + t1280 + t1323 + t1370 + t1411 + t1450);
	/****************************************************************************************/
	t1 = n * n;
	t2 = t1 * xc;
	t3 = ZB * ZB;
	t5 = t2 * 0.3141592654e1 * t3;
	t6 = nx * 0.3141592654e1;
	t7 = t6 * xc;
	t8 = cos(t7);
	t9 = nx * nx;
	t10 = t8 * t9;
	t11 = n * 0.3141592654e1;
	t12 = exp(t11);
	t13 = t12 * t12;
	t16 = exp(xc * n * 0.3141592654e1);
	t17 = t16 * t16;
	t18 = t17 * t16;
	t19 = t17 * t17;
	t20 = t19 * t18;
	t21 = t13 * t20;
	t22 = t10 * t21;
	t25 = ZA * ZA;
	t26 = t1 * t25;
	t27 = xc * 0.3141592654e1;
	t28 = t26 * t27;
	t29 = t19 * t16;
	t30 = t13 * t13;
	t31 = t29 * t30;
	t35 = t9 * nx;
	t36 = t3 * t35;
	t37 = sin(t6);
	t38 = t13 * t12;
	t39 = t37 * t38;
	t40 = t39 * t19;
	t42 = t1 * t1;
	t43 = nx * t42;
	t44 = xc * xc;
	t45 = t25 * t44;
	t46 = t43 * t45;
	t47 = 0.3141592654e1 * 0.3141592654e1;
	t48 = t47 * t37;
	t49 = t17 * t38;
	t54 = 0.3141592654e1 * t35;
	t55 = ZA * n * t54;
	t56 = t37 * ZB;
	t57 = t19 * t12;
	t61 = t25 * t8;
	t62 = t61 * t9;
	t63 = n * t30;
	t64 = t63 * t16;
	t67 = t1 * n;
	t69 = t47 * ZB;
	t70 = t67 * t44 * t69;
	t75 = nx * t3;
	t76 = t75 * t37;
	t77 = t67 * 0.3141592654e1;
	t78 = t19 * t19;
	t79 = t78 * t12;
	t80 = t77 * t79;
	t82 = t3 * t38;
	t84 = t54 * t37;
	t87 = sin(t7);
	t88 = t29 * t87;
	t89 = t47 * t44;
	t93 = nx * t25;
	t94 = t87 * t42;
	t95 = t93 * t94;
	t96 = t47 * xc;
	t97 = t13 * t29;
	t98 = t96 * t97;
	t101 = t87 * t67;
	t102 = t93 * t101;
	t103 = t13 * t18;
	t107 = t47 * t35;
	t108 = t26 * t107;
	t109 = t37 * t44;
	t110 = t19 * t17;
	t111 = t12 * t110;
	t116 = t37 * t19 * t12;
	t118 = t37 * xc;
	t119 = ZB * t19;
	t120 = t119 * t12;
	t121 = t118 * t120;
	t125 = xc * t3;
	t126 = t1 * t47 * t125;
	t127 = t35 * t37;
	t128 = t38 * t19;
	t129 = t127 * t128;
	t132 = t26 * 0.3141592654e1;
	t133 = t16 * t13;
	t134 = t10 * t133;
	t137 = 0.3141592654e1 * ZB;
	t138 = t2 * t137;
	t139 = ZA * t8;
	t140 = t9 * t13;
	t145 = t30 * t18;
	t146 = t10 * t145;
	t149 = t3 * t8;
	t150 = t149 * t9;
	t153 = 0.2e1 * t5 * t22 + 0.2e1 * t28 * t10 * t31 + t36 * t40 - 0.2e1 * t46 * t48 * t49 - 0.2e1 * t55 * t56 * t57 - 0.2e1 * t62 * t64 + 0.16e2 * t70 * t29 * ZA * t10 - t76 * t80 + t82 * n * t84 + 0.8e1 * t43 * t3 * t88 * t89 + 0.16e2 * t95 * t98 + 0.2e1 * t102 * t27 * t103 - 0.2e1 * t108 * t109 * t111 + t36 * t116 + 0.8e1 * t55 * t121 - 0.4e1 * t126 * t129 - 0.4e1 * t132 * t134 - 0.4e1 * t138 * t139 * t140 * t20 + 0.8e1 * t28 * t146 - 0.2e1 * t150 * t64;
	t154 = t42 * n;
	t155 = nx * t154;
	t156 = t44 * xc;
	t157 = t47 * 0.3141592654e1;
	t158 = t156 * t157;
	t159 = t155 * t158;
	t162 = t56 * ZA * t19 * t12;
	t165 = t77 * t49;
	t167 = t1 * t3;
	t168 = t167 * t89;
	t169 = t127 * t49;
	t172 = t37 * t67;
	t173 = t75 * t172;
	t174 = t38 * t110;
	t175 = t27 * t174;
	t179 = t47 * t25;
	t181 = t10 * t97;
	t184 = t27 * t31;
	t187 = t67 * t47;
	t188 = t44 * t3;
	t189 = t187 * t188;
	t192 = t25 * t35;
	t193 = t37 * t17;
	t194 = t193 * t12;
	t196 = nx * ZA;
	t197 = t196 * t172;
	t198 = ZB * t38;
	t199 = t198 * t19;
	t204 = t1 * t12 * t110;
	t207 = nx * ZB;
	t209 = t1 * ZA;
	t215 = t67 * t3;
	t216 = t47 * t8;
	t217 = t215 * t216;
	t218 = t9 * xc;
	t222 = nx * t67;
	t223 = t222 * t27;
	t224 = t3 * t87;
	t228 = t167 * t107;
	t232 = t26 * t96;
	t235 = t207 * t94;
	t236 = t47 * ZA;
	t243 = xc * t13;
	t244 = t243 * t29;
	t248 = t25 * n;
	t249 = t248 * 0.3141592654e1;
	t253 = ZB * ZA;
	t254 = t253 * t8;
	t255 = t9 * n;
	t256 = t30 * t16;
	t260 = 0.2e1 * t207 * t37 * t209 * t128 + 0.2e1 * t5 * t134 - 0.16e2 * t217 * t218 * t97 - 0.2e1 * t223 * t224 * t31 - 0.2e1 * t228 * t109 * t174 - 0.2e1 * t232 * t169 - 0.16e2 * t235 * t236 * t44 * t30 * t18 - 0.4e1 * t196 * t101 * t137 * t244 + t249 * t169 + 0.8e1 * t168 * t129 + 0.4e1 * t254 * t255 * t256;
	t263 = t43 * t179;
	t267 = t3 * n;
	t268 = t267 * t54;
	t269 = t118 * t57;
	t272 = t39 * t1;
	t274 = t67 * t25;
	t275 = t274 * t158;
	t278 = t75 * t87;
	t279 = t77 * t103;
	t282 = t25 * t38;
	t285 = ZA * t38;
	t290 = t267 * 0.3141592654e1;
	t296 = t77 * t111;
	t298 = t196 * t37;
	t299 = t1 * ZB;
	t303 = t37 * t42;
	t304 = t196 * t303;
	t308 = t77 * t57;
	t310 = t26 * t89;
	t313 = t77 * t128;
	t316 = t101 * t27;
	t319 = t93 * t87;
	t320 = t77 * t97;
	t323 = t127 * t57;
	t326 = t10 * n;
	t329 = t118 * t174;
	t332 = -0.8e1 * t263 * t109 * t57 - 0.4e1 * t268 * t269 + t93 * t272 + 0.8e1 * t275 * t129 - 0.4e1 * t278 * t279 + t282 * n * t84 - 0.2e1 * t285 * n * t54 * t56 - t290 * t169 - 0.2e1 * t196 * t38 * t172 * t137 + t76 * t296 - 0.2e1 * t298 * t299 * t79 + 0.8e1 * t304 * t96 * t120 + t76 * t308 - 0.2e1 * t310 * t169 - t76 * t313 + 0.2e1 * t75 * t18 * t316 + 0.4e1 * t319 * t320 + t249 * t323 - 0.2e1 * t25 * t18 * t326 + 0.2e1 * t228 * t329;
	t335 = t75 * t101;
	t336 = t27 * t21;
	t342 = t77 * t133;
	t347 = t209 * t137;
	t350 = t9 * t1;
	t351 = t149 * t350;
	t355 = t37 * t78 * t12;
	t359 = t93 * t303;
	t367 = t172 * 0.3141592654e1;
	t369 = t96 * t103;
	t376 = t209 * t107;
	t379 = t10 * t103;
	t383 = t207 * t101;
	t389 = 0.3141592654e1 * ZA;
	t390 = t222 * t389;
	t391 = t87 * ZB;
	t398 = -0.2e1 * t102 * t336 + t93 * t38 * t367 + 0.16e2 * t95 * t369 - t82 * t127 - 0.8e1 * t197 * t27 * t120 + 0.8e1 * t376 * t121 - 0.8e1 * t189 * t379 - t249 * t129 - 0.4e1 * t383 * t27 * ZA * t16 * t13 - 0.8e1 * t390 * t391 * t21 - 0.2e1 * t197 * t137 * t57;
	t402 = t39 * t110;
	t404 = t193 * t38;
	t406 = t127 * t174;
	t408 = t167 * 0.3141592654e1;
	t411 = t44 * t157;
	t412 = t155 * t411;
	t413 = t285 * t19;
	t414 = t56 * t413;
	t417 = ZA * t30;
	t424 = t93 * t37;
	t426 = t248 * t54;
	t427 = t17 * t12;
	t428 = t118 * t427;
	t431 = t77 * t21;
	t438 = ZA * t13;
	t443 = t93 * t172;
	t444 = t27 * t427;
	t448 = t1 * t78 * t12;
	t455 = t274 * t89;
	t461 = t118 * t111;
	t464 = -t36 * t402 + t36 * t404 - t249 * t406 - 0.4e1 * t408 * t134 + 0.16e2 * t412 * t414 - 0.4e1 * t383 * t27 * t417 * t18 + 0.2e1 * t28 * t22 - t424 * t80 - 0.2e1 * t426 * t428 + 0.4e1 * t278 * t431 + 0.4e1 * t254 * t255 * t103 + t290 * t323 + 0.4e1 * t383 * t27 * t438 * t20 + 0.2e1 * t443 * t444 + t424 * t448 - t36 * t194 - 0.32e2 * t235 * t236 * t243 * t18 + 0.8e1 * t455 * t181 - 0.4e1 * t359 * t96 * t128 - 0.2e1 * t426 * t461;
	t469 = n * t16 * t13;
	t474 = t1 * t38;
	t475 = t474 * t19;
	t480 = t89 * t103;
	t483 = t67 * ZA;
	t484 = t483 * t411;
	t485 = t127 * t120;
	t488 = t127 * t111;
	t497 = t77 * t427;
	t502 = t27 * t97;
	t508 = t1 * t19 * t12;
	t511 = t155 * t25 * t156;
	t512 = t157 * t37;
	t513 = t512 * t128;
	t527 = t1 * t17;
	t528 = t527 * t38;
	t530 = -t76 * t497 - 0.4e1 * t254 * t255 * t97 - 0.2e1 * t102 * t502 - 0.4e1 * t108 * t269 - t76 * t508 + 0.8e1 * t511 * t513 + 0.4e1 * t150 * t63 * t18 + 0.4e1 * t383 * t27 * t438 * t18 + 0.4e1 * t132 * t379 + 0.2e1 * t168 * t488 - t76 * t528;
	t535 = t44 * t13;
	t542 = t527 * t12;
	t544 = n * t13;
	t545 = t544 * t20;
	t548 = t75 * t303;
	t549 = t96 * t111;
	t552 = ZA * t35;
	t553 = t552 * t37;
	t562 = t43 * t96;
	t563 = t3 * t37;
	t564 = t563 * t128;
	t579 = t474 * t110;
	t590 = t9 * t30;
	t591 = t590 * t18;
	t595 = t127 * t427;
	t598 = t77 * t174;
	t600 = 0.4e1 * t5 * t146 + 0.16e2 * t235 * t236 * t535 * t18 + 0.8e1 * t455 * t146 + t76 * t542 - 0.2e1 * t150 * t545 + 0.2e1 * t548 * t549 - 0.2e1 * t553 * t120 + t290 * t488 - 0.8e1 * t274 * t47 * t44 * t29 * t10 - 0.4e1 * t562 * t564 - 0.2e1 * t132 * xc * t20 * t10 - 0.32e2 * t562 * ZA * t87 * ZB * t13 * t29 - 0.8e1 * t347 * t379 + t76 * t579 - 0.4e1 * t359 * t96 * t57 + 0.4e1 * t408 * t181 - 0.4e1 * t223 * t564 - 0.12e2 * t209 * t27 * ZB * t8 * t591 + 0.2e1 * t310 * t595 + t76 * t598;
	t601 = t27 * t49;
	t604 = t127 * t79;
	t606 = ZB * t29;
	t616 = t139 * t140 * t18;
	t638 = t10 * t256;
	t643 = t118 * t199;
	t653 = t544 * t29;
	t658 = t3 * t29;
	t660 = t350 * t27;
	t663 = -0.4e1 * t254 * t255 * t145 + 0.2e1 * t267 * t20 * t8 * t9 - 0.4e1 * t138 * t139 * t9 * t16 * t13 - 0.2e1 * t5 * t638 + 0.2e1 * t126 * t169 + 0.8e1 * t376 * t643 + 0.4e1 * t335 * t27 * t145 + 0.16e2 * t235 * t236 * t535 * t29 + 0.6e1 * t150 * t653 - 0.4e1 * t426 * t269 + 0.4e1 * t658 * t8 * t660;
	t670 = t274 * t411;
	t673 = t118 * t49;
	t694 = t155 * t45;
	t713 = n * t29 * t30;
	t717 = t20 * t87;
	t723 = t512 * t57;
	t728 = -0.2e1 * t443 * t175 - 0.8e1 * t670 * t129 + 0.2e1 * t426 * t673 - 0.16e2 * t207 * t88 * t42 * t47 * ZA * t44 + 0.4e1 * t254 * t255 * t21 + t249 * t595 + 0.8e1 * t25 * t29 * t8 * t660 + 0.2e1 * t268 * t461 + 0.8e1 * t189 * t181 - 0.8e1 * t694 * t513 + 0.2e1 * t198 * t553 - 0.12e2 * t606 * t139 * t660 - 0.2e1 * t359 * t549 + 0.4e1 * t138 * t139 * t590 * t16 + 0.8e1 * t93 * t29 * t94 * t89 - 0.2e1 * t150 * t713 + 0.2e1 * t222 * t3 * t717 * t27 + 0.8e1 * t670 * t323 + 0.8e1 * t694 * t723 - 0.2e1 * t62 * t653;
	t734 = t43 * t89;
	t735 = t563 * t427;
	t740 = t75 * t94;
	t744 = ZB * xc;
	t750 = t563 * t57;
	t754 = t218 * t103;
	t771 = t127 * t199;
	t776 = t89 * t174;
	t791 = -0.4e1 * t207 * t717 * t77 * xc * ZA + 0.4e1 * t443 * t27 * t57 + t192 * t40 - 0.8e1 * t55 * t643 - 0.16e2 * t209 * t89 * t771 - 0.8e1 * t275 * t323 + 0.2e1 * t359 * t776 + 0.16e2 * t304 * t89 * t199 + 0.4e1 * t278 * t320 + 0.2e1 * t207 * t172 * t389 * t79 - 0.8e1 * t390 * t391 * t97;
	t794 = t483 * t158;
	t801 = t2 * 0.3141592654e1;
	t818 = t215 * t411;
	t827 = t96 * t174;
	t837 = t37 * t12 * t110;
	t845 = 0.16e2 * t794 * t485 + 0.8e1 * t159 * t564 - 0.8e1 * t455 * t379 - 0.2e1 * t801 * t3 * t20 * t10 - 0.4e1 * t132 * t22 - 0.8e1 * t734 * t564 - 0.8e1 * t187 * t44 * t658 * t10 - 0.8e1 * t412 * t564 + 0.4e1 * t132 * t181 - 0.8e1 * t818 * t129 + 0.2e1 * t46 * t48 * t427 - 0.4e1 * t75 * t29 * t316 - 0.2e1 * t359 * t827 - t290 * t595 + 0.16e2 * t217 * t754 - t424 * t542 - 0.8e1 * t734 * t750 - t192 * t837 - 0.4e1 * t254 * t255 * t133 + 0.8e1 * t304 * t96 * t199;
	t864 = t544 * t18;
	t867 = t3 * t18;
	t884 = t27 * t256;
	t891 = t187 * t744;
	t894 = t563 * t49;
	t900 = -0.2e1 * t263 * t428 + 0.2e1 * t228 * t428 - 0.6e1 * t223 * t224 * t103 - t192 * t404 + 0.2e1 * t268 * t428 - 0.2e1 * t335 * t884 - t424 * t296 + 0.2e1 * t93 * t20 * t316 - 0.32e2 * t891 * t616 + 0.2e1 * t562 * t894 - 0.2e1 * t801 * t867 * t10;
	t904 = t27 * t111;
	t907 = t118 * t128;
	t915 = t89 * t145;
	t947 = t139 * t140 * t29;
	t952 = -0.2e1 * t173 * t904 + 0.4e1 * t426 * t907 + 0.12e2 * t253 * t10 * t1 * 0.3141592654e1 * t244 + 0.8e1 * t95 * t915 - t36 * t355 - 0.16e2 * t794 * t771 - 0.8e1 * t511 * t723 + 0.16e2 * t734 * t162 + t36 * t837 + 0.2e1 * t298 * t299 * t57 - 0.2e1 * t28 * t638 - 0.2e1 * t62 * t545 + 0.2e1 * t310 * t406 + 0.12e2 * t138 * t616 + 0.4e1 * t223 * t750 + t424 * t497 + 0.2e1 * t734 * t894 + 0.2e1 * t132 * xc * t18 * t10 - 0.16e2 * t70 * t947 + 0.32e2 * t891 * t947;
	t969 = t67 * t157 * t156 * t3;
	t974 = t27 * t133;
	t1001 = -0.8e1 * t159 * t750 - 0.16e2 * t412 * t162 - t290 * t129 + 0.8e1 * t310 * t323 - 0.4e1 * t319 * t342 + t75 * t272 + t192 * t402 - 0.8e1 * t359 * t89 * t128 - 0.10e2 * t61 * t350 * t502 + 0.8e1 * t818 * t323 - 0.4e1 * t108 * t907;
	t1042 = t89 * t97;
	t1055 = -0.2e1 * t168 * t595 + 0.16e2 * t484 * t771 + 0.4e1 * t11 * t125 * t129 - 0.2e1 * t173 * t444 + 0.2e1 * ZB * n * t54 * t37 * ZA * t79 - t424 * t475 + 0.2e1 * t562 * t735 - 0.2e1 * t548 * t776 + t424 * t204 + 0.2e1 * t25 * t20 * t326 + 0.8e1 * t383 * t389 * t133 + t75 * t38 * t367 + 0.2e1 * t62 * t469 + 0.2e1 * t197 * t137 * t128 - 0.2e1 * t102 * t884 - 0.2e1 * t5 * t379 - 0.8e1 * t740 * t1042 - 0.16e2 * t159 * t414 - 0.2e1 * ZB * t35 * t37 * t413 + 0.2e1 * t553 * ZB * t78 * t12;
	t1096 = 0.2e1 * t443 * t904 - 0.2e1 * t268 * t329 - 0.2e1 * t443 * t601 + 0.2e1 * t102 * t974 - 0.2e1 * t263 * t673 + t424 * t165 + 0.2e1 * t62 * t713 + t424 * t308 - t424 * t313 + 0.8e1 * t347 * t22 - t424 * t598;
	t1103 = t42 * t1 * t157;
	t1104 = t1103 * t25;
	t1108 = t3 * t19;
	t1112 = n * t47;
	t1113 = t9 * t9;
	t1118 = t42 * t157;
	t1119 = t1118 * t9;
	t1120 = t25 * xc;
	t1121 = t13 * t110;
	t1122 = t1120 * t1121;
	t1125 = t47 * t47;
	t1126 = t67 * t1125;
	t1127 = t1113 * ZA;
	t1128 = t1126 * t1127;
	t1129 = t19 * t13;
	t1130 = t744 * t1129;
	t1133 = t154 * t1125;
	t1134 = t1133 * t9;
	t1135 = t45 * t1129;
	t1138 = t154 * t47;
	t1139 = t25 * t30;
	t1142 = t1126 * t1113;
	t1145 = t125 * t1129;
	t1148 = t1103 * xc;
	t1149 = t3 * t13;
	t1150 = t1149 * t17;
	t1153 = t25 * t78;
	t1156 = -0.8e1 * t1104 * t243 * t17 + 0.4e1 * t187 * t1108 * t9 - 0.2e1 * t1112 * t3 * t1113 * t30 + 0.16e2 * t1119 * t1122 + 0.64e2 * t1128 * t1130 + 0.64e2 * t1134 * t1135 - 0.2e1 * t1138 * t1139 + 0.32e2 * t1142 * t1135 - 0.32e2 * t1142 * t1145 + 0.8e1 * t1148 * t1150 - 0.2e1 * t1138 * t1153;
	t1157 = t25 * t13;
	t1158 = t1157 * t17;
	t1161 = t13 * t17;
	t1162 = t1120 * t1161;
	t1165 = t3 * t78;
	t1170 = t42 * t67 * t1125;
	t1172 = t1108 * t13;
	t1175 = t1 * t157;
	t1176 = t1175 * t1113;
	t1182 = t1120 * t1129;
	t1189 = t110 * t9 * xc;
	t1192 = t1149 * t110;
	t1201 = 0.8e1 * t1103 * t1158 - 0.16e2 * t1119 * t1162 - 0.2e1 * t1112 * t1165 * t1113 + 0.32e2 * t1170 * t44 * t1172 - 0.8e1 * t1176 * t1162 + 0.8e1 * t1104 * t243 * t110 - 0.64e2 * t1134 * t1182 - 0.64e2 * t1134 * t1145 + 0.16e2 * t1118 * t3 * t1189 + 0.16e2 * t1119 * t1192 - 0.4e1 * t187 * t1165 * t9 - 0.4e1 * t187 * t1139 * t9;
	t1209 = t17 * t30;
	t1210 = t125 * t1209;
	t1213 = t1138 * ZA;
	t1214 = ZB * t30;
	t1218 = t1157 * t110;
	t1226 = t3 * t30;
	t1237 = t1170 * t25;
	t1242 = 0.4e1 * t1112 * ZA * t119 * t1113 - 0.16e2 * t1119 * t1150 - 0.8e1 * t1176 * t1210 + 0.4e1 * t1213 * t1214 * t19 - 0.16e2 * t1119 * t1218 - 0.32e2 * t1142 * t1182 - 0.8e1 * t1103 * t1120 * t110 - 0.4e1 * t187 * t1226 * t9 + 0.8e1 * t1103 * t1192 + 0.4e1 * t1112 * ZB * t1113 * t30 * ZA - 0.32e2 * t1237 * xc * t19 * t13;
	t1251 = t125 * t1121;
	t1260 = t1120 * t1209;
	t1263 = t1139 * t19;
	t1282 = 0.8e1 * t1103 * t110 * t3 * xc + 0.8e1 * t1104 * xc * t17 * t30 - 0.8e1 * t1176 * t1251 + 0.16e2 * t1119 * t1158 + 0.4e1 * t1112 * t78 * t1127 * ZB + 0.16e2 * t1119 * t1260 + 0.2e1 * t1138 * t1263 - 0.32e2 * t1170 * xc * t1172 - 0.16e2 * t1213 * t119 * t13 + 0.4e1 * t1138 * t1214 * ZA + 0.32e2 * t1237 * t44 * t19 * t13 - 0.16e2 * t1118 * t25 * t1189;
	t1287 = t188 * t1129;
	t1292 = t25 * t19;
	t1296 = t187 * t9;
	t1297 = t1226 * t19;
	t1311 = t1112 * t1113;
	t1317 = -0.8e1 * t1176 * t1150 + 0.32e2 * t1142 * t1287 - 0.8e1 * t1103 * t1150 + 0.2e1 * t1112 * t1292 * t1113 + 0.4e1 * t1296 * t1297 + 0.8e1 * t1176 * t1192 + 0.4e1 * t1296 * t1263 + 0.8e1 * t1176 * t1158 - 0.8e1 * t1175 * t25 * t1113 * xc * t110 + 0.2e1 * t1311 * t1297 + 0.2e1 * t1112 * t1108 * t1113;
	t1320 = t253 * t1129;
	t1328 = t253 * t30 * t19;
	t1333 = t125 * t1161;
	t1343 = ZB * t44 * t1129;
	t1350 = -0.8e1 * t1176 * t1218 - 0.16e2 * t1311 * t1320 + 0.8e1 * t1176 * t1260 - 0.16e2 * t1119 * t1210 + 0.4e1 * t1311 * t1328 + 0.2e1 * t1311 * t1263 + 0.8e1 * t1176 * t1333 + 0.8e1 * t187 * ZB * t417 * t9 - 0.2e1 * t1138 * t1165 - 0.64e2 * t1128 * t1343 + 0.64e2 * t1134 * t1287 + 0.2e1 * t1138 * t1108;
	t1369 = t1133 * t9 * ZA;
	t1378 = t187 * ZA;
	t1383 = t1170 * ZA;
	t1388 = 0.2e1 * t1138 * t1297 - 0.8e1 * t1148 * t1192 + 0.2e1 * t1138 * t1292 - 0.16e2 * t1119 * t1251 + 0.8e1 * t1175 * xc * t110 * t1113 * t3 - 0.2e1 * t1112 * t1153 * t1113 + 0.128e3 * t1369 * t1130 + 0.16e2 * t1119 * t1333 + 0.4e1 * t1138 * t78 * ZA * ZB + 0.8e1 * t1378 * t78 * t9 * ZB - 0.64e2 * t1383 * t1343 + 0.64e2 * t1383 * t1130;
	t1420 = 0.4e1 * t1138 * t119 * ZA - 0.128e3 * t1369 * t1343 - 0.4e1 * t187 * t1153 * t9 - 0.2e1 * t1138 * t1226 + 0.8e1 * t1296 * t1328 - 0.2e1 * t1112 * t1139 * t1113 - 0.8e1 * t1148 * t3 * t17 * t30 - 0.32e2 * t1296 * t1320 + 0.8e1 * t1176 * t1122 + 0.4e1 * t187 * t1292 * t9 + 0.8e1 * t1378 * t119 * t9 - 0.8e1 * t1103 * t1218;

	_C4B = (-t424 * t508 + 0.8e1 * t412 * t750 - 0.2e1 * t232 * t595 - 0.4e1 * t126 * t323 + t1096 - t76 * t204 + t728 + 0.2e1 * t548 * t827 + 0.2e1 * t150 * t469 + t398 + 0.8e1 * t189 * t146 + t260 - 0.2e1 * t351 * t184 - 0.2e1 * t268 * t673 - 0.4e1 * t319 * t279 + t464 - 0.2e1 * t108 * t461 + 0.16e2 * t740 * t369 + 0.16e2 * t274 * t216 * t754 - 0.16e2 * t70 * t139 * t591 + 0.2e1 * t55 * t56 * t128 - 0.2e1 * t359 * t89 * t111 + 0.2e1 * t734 * t563 * t111 + 0.6e1 * t223 * t224 * t97 + 0.8e1 * t383 * t389 * t103 + 0.4e1 * t606 * ZA * t326 - 0.2e1 * t93 * t18 * t316 - 0.4e1 * t443 * t27 * t128 + 0.8e1 * t197 * t27 * t199 + 0.8e1 * t108 * t109 * t128 - t249 * t604 + 0.16e2 * t70 * t616 - 0.8e1 * t969 * t323 + t845 - t424 * t579 + 0.16e2 * t159 * t162 + t290 * t406 - 0.6e1 * t150 * t864 + t192 * t116 + 0.2e1 * t867 * t326 - 0.4e1 * t658 * t326 - 0.2e1 * t351 * t502 - t76 * t165 + t900 + 0.8e1 * t168 * t323 + t791 + 0.8e1 * t740 * t915 - 0.4e1 * t562 * t750 - 0.4e1 * t278 * t342 + 0.4e1 * t319 * t431 + 0.2e1 * t173 * t175 + t424 * t528 + 0.8e1 * t969 * t129 - 0.8e1 * t347 * t181 + t332 + t530 - 0.2e1 * t108 * t329 - 0.2e1 * t207 * t38 * t37 * t1 * ZA + t1001 + 0.4e1 * t408 * t379 + t76 * t448 + 0.2e1 * t102 * t184 + 0.2e1 * t426 * t329 + 0.16e2 * t740 * t98 - t282 * t127 - 0.16e2 * t1 * t44 * t69 * t552 * t116 + 0.2e1 * t168 * t169 + 0.2e1 * t28 * t134 - t290 * t604 - 0.16e2 * t484 * t485 - 0.8e1 * t740 * t480 + 0.2e1 * t173 * t601 - 0.2e1 * t335 * t336 + t600 + 0.2e1 * t62 * t864 + t952 + 0.8e1 * t347 * t134 - t192 * t355 + t192 * t194 + 0.2e1 * t228 * t461 + t663 + 0.4e1 * t383 * t27 * t417 * t16 + 0.4e1 * t138 * t20 * ZA * t10 - 0.4e1 * t20 * ZB * ZA * t326 + 0.4e1 * t196 * t88 * t77 * t744 - 0.16e2 * t67 * xc * t179 * t181 - 0.8e1 * t95 * t480 - t249 * t488 - t76 * t475 + t1055 - 0.4e1 * t408 * t22 - 0.10e2 * t28 * t379 + 0.2e1 * t335 * t974 + t153 - 0.8e1 * t95 * t1042 - 0.2e1 * t734 * t735) / (t1156 + t1201 + t1242 + t1282 + t1317 + t1350 + t1388 + t1420);
	/****************************************************************************************/
	/****************************************************************************************/

	if(x>xc) {
		_C1=_C1B; _C2=_C2B; _C3=_C3B; _C4=_C4B; Z=ZB;
	}
	else {
		_C1=_C1A; _C2=_C2A; _C3=_C3A; _C4=_C4A; Z=ZA;
	}
	/****************************************************************************************/
	/****************************************************************************************/
	t1 = n * n;
	t2 = t1 * t1;
	t3 = t2 * n;
	t4 = x * t3;
	t5 = 0.3141592654e1 * 0.3141592654e1;
	t6 = t5 * 0.3141592654e1;
	t11 = _C3 * t6;
	t12 = x * n;
	t13 = nx * nx;
	t14 = t13 * t13;
	t15 = t12 * t14;
	t19 = exp(t12 * 0.3141592654e1);
	t20 = t19 * t19;
	t21 = t4 * t20;
	t24 = _C1 * t5;
	t25 = Z * t20;
	t29 = _C1 * t6;
	t30 = t29 * Z;
	t31 = t1 * n;
	t32 = x * t31;
	t33 = t32 * t13;
	t36 = t11 * x;
	t41 = n * t20;
	t45 = t6 * _C4;
	t49 = t20 * t1;
	t51 = _C2 * Z;
	t55 = -0.2e1 * t4 * t6 * _C2 * Z - 0.2e1 * t11 * t15 - 0.2e1 * t11 * t21 + 0.2e1 * t24 * t25 * t14 - t13 + 0.4e1 * t30 * t33 - 0.4e1 * t36 * t31 * t20 * t13 - 0.2e1 * t36 * t41 * t14 - 0.2e1 * t4 * t45 * t20 - t49 - 0.2e1 * t4 * t6 * t51 * t20;
	t58 = t32 * t6;
	t59 = _C4 * t20;
	t63 = t20 * t13;
	t67 = t12 * t6;
	t68 = t20 * t14;
	t87 = t49 * t13;
	t90 = -0.4e1 * t11 * t33 - 0.4e1 * t58 * t59 * t13 - 0.4e1 * t58 * t51 * t63 - 0.2e1 * t67 * t51 * t68 + 0.4e1 * t32 * t45 * t13 - 0.2e1 * t67 * t59 * t14 - 0.2e1 * t30 * t21 + t1 + 0.2e1 * t24 * t25 * t2 + 0.2e1 * t12 * t45 * t14 + 0.4e1 * t24 * Z * t87;
	t106 = _C3 * t5;
	t120 = -0.4e1 * t30 * t32 * t63 + t63 + 0.4e1 * t24 * Z * t1 * t13 + 0.2e1 * t29 * Z * x * t3 - 0.4e1 * t58 * t51 * t13 - 0.2e1 * t106 * t2 + t32 * 0.3141592654e1 - 0.2e1 * t106 * t14 - 0.2e1 * t30 * t12 * t68 - 0.2e1 * t67 * t51 * t14 + 0.4e1 * t106 * t87;
	t129 = sin(nx * 0.3141592654e1 * x);
	t155 = 0.2e1 * t30 * t15 + x * 0.3141592654e1 * t41 * t13 - 0.4e1 * t19 * nx * t129 * n + t32 * 0.3141592654e1 * t20 + 0.2e1 * t106 * t68 + 0.2e1 * t106 * t20 * t2 - 0.4e1 * t106 * t1 * t13 - 0.2e1 * t11 * t4 + 0.2e1 * t4 * t45 + 0.2e1 * t24 * Z * t2 + 0.2e1 * t24 * Z * t14 + t12 * 0.3141592654e1 * t13;
	t158 = t5 * Z;

	u1 = (t55 + t90 + t120 + t155) / (0.4e1 * t158 * t19 * t2 + 0.8e1 * t158 * t19 * t1 * t13 + 0.4e1 * t158 * t19 * t14);
	/****************************************************************************************/
	/****************************************************************************************/
	t1 = n * n;
	t2 = t1 * n;
	t3 = x * t2;
	t4 = 0.3141592654e1 * 0.3141592654e1;
	t5 = t4 * 0.3141592654e1;
	t6 = t3 * t5;
	t7 = _C2 * Z;
	t8 = nx * nx;
	t12 = t1 * t1;
	t13 = t12 * n;
	t14 = x * t13;
	t15 = t5 * _C4;
	t16 = x * n;
	t18 = exp(t16 * 0.3141592654e1);
	t19 = t18 * t18;
	t23 = t16 * t5;
	t24 = t8 * t8;
	t28 = _C3 * t5;
	t29 = t14 * t19;
	t32 = _C1 * t5;
	t33 = t32 * Z;
	t34 = t16 * t24;
	t37 = _C4 * t19;
	t45 = _C2 * t4;
	t53 = t19 * t8;
	t58 = _C4 * t4;
	t60 = t1 * t19 * t8;
	t63 = t19 * t24;
	t67 = t3 * t8;
	t73 = n * t19;
	t86 = t28 * x;
	t91 = 0.4e1 * t58 * t60 + 0.2e1 * t33 * t16 * t63 + 0.4e1 * t33 * t67 + 0.2e1 * t33 * t29 - x * 0.3141592654e1 * t73 * t8 - 0.2e1 * t53 + 0.2e1 * t32 * Z * x * t13 - 0.2e1 * t58 * t12 - 0.2e1 * t58 * t24 + t3 * 0.3141592654e1 + 0.4e1 * t86 * t2 * t19 * t8;
	t94 = Z * t12;
	t121 = -0.2e1 * t8 + 0.2e1 * t45 * t94 * t19 + 0.2e1 * t14 * t5 * t7 * t19 + 0.4e1 * t6 * t7 * t53 + 0.2e1 * t23 * t7 * t63 - 0.4e1 * t28 * t67 + 0.2e1 * t45 * t94 + 0.2e1 * t58 * t12 * t19 + t16 * 0.3141592654e1 * t8 + 0.2e1 * t14 * t15 - 0.2e1 * t28 * t14;
	t146 = cos(nx * 0.3141592654e1 * x);
	t156 = -t3 * 0.3141592654e1 * t19 + 0.2e1 * t58 * t63 - 0.4e1 * t58 * t1 * t8 + 0.4e1 * t45 * Z * t1 * t8 - 0.2e1 * t28 * t34 + 0.2e1 * t86 * t73 * t24 + 0.4e1 * t3 * t15 * t8 + 0.4e1 * t45 * Z * t60 + 0.4e1 * t18 * t146 * t8 + 0.2e1 * t45 * Z * t24 + 0.2e1 * t16 * t15 * t24;
	t159 = t4 * Z;

	u2 = (-0.4e1 * t6 * t7 * t8 + 0.2e1 * t14 * t15 * t19 - 0.2e1 * t23 * t7 * t24 + 0.2e1 * t28 * t29 + 0.2e1 * t33 * t34 + 0.4e1 * t6 * t37 * t8 - 0.2e1 * t14 * t5 * _C2 * Z + 0.2e1 * t45 * Z * t19 * t24 + 0.2e1 * t23 * t37 * t24 + 0.4e1 * t33 * t3 * t53 + t91 + t121 + t156) / (0.4e1 * t159 * t18 * t12 + 0.8e1 * t159 * t18 * t1 * t8 + 0.4e1 * t159 * t18 * t24);
	/****************************************************************************************/
	/****************************************************************************************/
	t1 = 0.3141592654e1 * 0.3141592654e1;
	t2 = t1 * 0.3141592654e1;
	t3 = _C1 * t2;
	t4 = t3 * Z;
	t5 = n * n;
	t6 = t5 * t5;
	t7 = t6 * n;
	t8 = x * t7;
	t9 = x * n;
	t11 = exp(t9 * 0.3141592654e1);
	t12 = t11 * t11;
	t13 = t8 * t12;
	t16 = t5 * n;
	t17 = x * t16;
	t18 = t17 * t2;
	t19 = _C4 * t12;
	t20 = nx * nx;
	t24 = t2 * _C4;
	t28 = _C3 * t2;
	t29 = t28 * x;
	t30 = t12 * n;
	t31 = t20 * t20;
	t40 = _C2 * Z;
	t44 = t9 * t2;
	t48 = t12 * t20;
	t52 = t17 * t20;
	t57 = -0.2e1 * t4 * t13 - 0.4e1 * t18 * t19 * t20 - 0.2e1 * t8 * t24 * t12 - 0.2e1 * t29 * t30 * t31 + 0.2e1 * t8 * t2 * _C2 * Z - 0.2e1 * t8 * t2 * t40 * t12 - 0.2e1 * t44 * t19 * t31 - 0.4e1 * t18 * t40 * t48 + t20 + 0.4e1 * t28 * t52 + t17 * 0.3141592654e1 * t12;
	t58 = t9 * t31;
	t61 = _C3 * t1;
	t62 = t12 * t31;
	t73 = t5 * t20;
	t78 = _C1 * t1;
	t90 = Z * t12;
	t94 = 0.2e1 * t28 * t58 + 0.2e1 * t61 * t62 + 0.2e1 * t61 * t12 * t6 - 0.4e1 * t4 * t17 * t48 + 0.2e1 * t28 * t8 + 0.4e1 * t61 * t73 - 0.2e1 * t8 * t24 - 0.2e1 * t78 * Z * t6 - 0.2e1 * t44 * t40 * t62 - 0.2e1 * t78 * Z * t31 - t9 * 0.3141592654e1 * t20 + 0.2e1 * t78 * t90 * t6;
	t101 = cos(nx * 0.3141592654e1 * x);
	t102 = t11 * t101;
	t109 = t12 * t5;
	t110 = t109 * t20;
	t128 = 0.2e1 * t61 * t6 - t17 * 0.3141592654e1 + 0.2e1 * t102 * t5 - 0.4e1 * t17 * t24 * t20 + 0.4e1 * t78 * Z * t110 - 0.2e1 * t9 * t24 * t31 - 0.4e1 * t4 * t52 - 0.2e1 * t4 * t9 * t62 + x * 0.3141592654e1 * t30 * t20 - t5 - 0.4e1 * t78 * Z * t5 * t20;
	t156 = 0.2e1 * t78 * t90 * t31 - 0.2e1 * t3 * Z * x * t7 + t48 + 0.4e1 * t61 * t110 + 0.4e1 * t18 * t40 * t20 - 0.2e1 * t102 * t20 + 0.2e1 * t61 * t31 + 0.2e1 * t44 * t40 * t31 - t109 - 0.2e1 * t4 * t58 - 0.2e1 * t28 * t13 - 0.4e1 * t29 * t16 * t12 * t20;
	t159 = t1 * t11;

	u3 = (t57 + t94 + t128 + t156) / (0.4e1 * t159 * t6 + 0.8e1 * t159 * t73 + 0.4e1 * t159 * t31);
	/****************************************************************************************/
	/****************************************************************************************/
	t1 = _C2 * Z;
	t2 = 0.3141592654e1 * 0.3141592654e1;
	t3 = t2 * 0.3141592654e1;
	t4 = n * n;
	t5 = t4 * t4;
	t6 = t5 * t4;
	t8 = t3 * t6 * x;
	t11 = x * t4;
	t12 = t11 * t3;
	t15 = exp(x * n * 0.3141592654e1);
	t16 = t15 * t15;
	t17 = _C3 * t16;
	t18 = nx * nx;
	t19 = t18 * t18;
	t23 = t5 * n;
	t24 = t2 * t23;
	t28 = t1 * t3;
	t29 = t6 * x;
	t30 = t29 * t16;
	t33 = _C4 * t3;
	t34 = t5 * x;
	t35 = t34 * t18;
	t41 = sin(nx * 0.3141592654e1 * x);
	t47 = t11 * t19;
	t54 = t3 * _C3;
	t57 = 0.2e1 * t1 * t8 + 0.2e1 * t12 * t17 * t19 + 0.2e1 * t1 * t24 * t16 + 0.2e1 * t28 * t30 - 0.4e1 * t33 * t35 + 0.2e1 * t15 * nx * t41 * t4 + 0.4e1 * t28 * t35 - 0.2e1 * t33 * t47 - 0.2e1 * t1 * t24 - 0.2e1 * t33 * t29 + 0.2e1 * t29 * t54;
	t58 = 0.3141592654e1 * t16;
	t60 = t2 * _C4;
	t69 = t4 * n;
	t73 = t1 * t2;
	t75 = t69 * t16 * t18;
	t79 = x * t16;
	t83 = n * t16;
	t84 = t83 * t19;
	t95 = -t34 * t58 + 0.2e1 * t60 * t23 * t16 + 0.2e1 * t60 * n * t19 - t11 * 0.3141592654e1 * t18 + 0.4e1 * t60 * t69 * t18 + 0.4e1 * t73 * t75 + 0.4e1 * t33 * t5 * t79 * t18 + 0.2e1 * t73 * t84 + 0.2e1 * t60 * t84 + 0.2e1 * t33 * t4 * t79 * t19 + 0.4e1 * t60 * t75;
	t97 = t34 * t3;
	t101 = Z * _C1;
	t102 = t16 * t19;
	t106 = t16 * t18;
	t127 = t2 * t69;
	t131 = t2 * n;
	t135 = 0.4e1 * t97 * t17 * t18 + 0.2e1 * t12 * t101 * t102 + 0.4e1 * t28 * t34 * t106 + 0.2e1 * t28 * t11 * t102 - 0.2e1 * t29 * t3 * Z * _C1 - 0.4e1 * t97 * t101 * t18 - 0.2e1 * t12 * t101 * t19 + 0.2e1 * t60 * t23 - 0.2e1 * t83 * t18 - 0.4e1 * t1 * t127 * t18 - 0.2e1 * t1 * t131 * t19;
	t164 = 0.2e1 * t28 * t47 + 0.2e1 * t11 * t54 * t19 + 0.2e1 * t8 * t101 * t16 + 0.2e1 * t33 * t30 - t11 * t58 * t18 + 0.2e1 * t29 * t54 * t16 + 0.4e1 * t34 * t54 * t18 + 0.4e1 * t97 * t101 * t106 - 0.2e1 * t15 * t18 * nx * t41 - t34 * 0.3141592654e1 + 0.2e1 * n * t18;

	u4 = (t57 + t95 + t135 + t164) / (0.4e1 * t24 * t15 + 0.8e1 * t127 * t15 * t18 + 0.4e1 * t131 * t15 * t19);


	/****************************************************************************************/
	/****************************************************************************************/


	u5 = (double)(-2*Z*n*PI*u2-u3*2*n*PI)*cos(n*PI*z); /* pressure */

	u6 = (double)(u3*2*n*PI + 4*Z*n*PI*u2)*cos(n*PI*z); /* zz stress */
	sum5 +=u5;
	sum6 +=u6;

	u1 *= cos(n*PI*z); /* x velocity */
	sum1 += u1;
	u2 *= sin(n*PI*z); /* z velocity */
	sum2 += u2;
	u3 *= 2*n*PI*cos(n*PI*z); /* xx stress */
	sum3 += u3;
	u4 *= 2*n*PI*sin(n*PI*z); /* zx stress */
	sum4 += u4;


	/*printf("%0.7f %0.7f %0.7f %0.7f %0.7f %0.7f %0.7f %0.7f %0.7f\n",x,z,sum1,sum2,sum3,sum4,sum5,sum6,mag);*/


	/* Output */
	if( vel != NULL ) {
		vel[0] = sum1;
		vel[1] = sum2;
	}
	if( presssure != NULL ) {
		(*presssure) = sum5;
	}
	if( total_stress != NULL ) {
		total_stress[0] = sum3;
		total_stress[1] = sum6;
		total_stress[2] = sum4;
	}
	if( strain_rate != NULL ) {
		if( x>xc ) {
			Z = ZB;
		}
		else {
			Z = ZA;
		}
		strain_rate[0] = (sum3+sum5)/(2.0*Z);
		strain_rate[1] = (sum6+sum5)/(2.0*Z);
		strain_rate[2] = (sum4+sum5)/(2.0*Z);
	}
}


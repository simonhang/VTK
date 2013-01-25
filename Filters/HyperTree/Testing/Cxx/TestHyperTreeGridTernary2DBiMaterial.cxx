/*==================================================================

  Program:   Visualization Toolkit
  Module:    TestHyperTreeGridTernary2DBiMaterial.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

===================================================================*/
// .SECTION Thanks
// This test was written by Philippe Pebay, Kitware 2013
// This work was supported in part by Commissariat a l'Energie Atomique (CEA/DIF)

#include "vtkHyperTreeGridGeometry.h"
#include "vtkHyperTreeGridSource.h"

#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkDataSetMapper.h"
#include "vtkNew.h"
#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkShrinkFilter.h"

int TestHyperTreeGridTernary2DBiMaterial( int argc, char* argv[] )
{
  // Hyper tree grids
  vtkNew<vtkHyperTreeGridSource> htGrid1;
  htGrid1->SetMaximumLevel( 3 );
  htGrid1->SetOrigin( 0., 0., 0. );
  htGrid1->SetGridSize( 2, 1, 1 );
  htGrid1->SetGridScale( 1., 1., 1. );
  htGrid1->SetDimension( 2 );
  htGrid1->SetBranchFactor( 3 );
  htGrid1->UseMaterialMaskOn();
  htGrid1->SetDescriptor( ".R|.R..R..R.|......... ......... ........." );
  htGrid1->SetMaterialMask( "..|..1..1..1|..1..1..1 ..1..1..1 ..1..1..1" );
  vtkNew<vtkHyperTreeGridSource> htGrid2;
  htGrid2->SetMaximumLevel( 3 );
  htGrid2->SetOrigin( 1., 0., 0. );
  htGrid2->SetGridSize( 2, 1, 1 );
  htGrid2->SetGridScale( 1., 1., 1. );
  htGrid2->SetDimension( 2 );
  htGrid2->SetBranchFactor( 3 );
  htGrid2->UseMaterialMaskOn();
  htGrid2->SetDescriptor( "R.|.R..R..R.|......... ......... ........." );
  htGrid2->SetMaterialMask( "..|1..1..1..|1..1..1.. 1..1..1.. 1..1..1.." );

  // Geometries
  vtkNew<vtkHyperTreeGridGeometry> geometry1;
  geometry1->SetInputConnection( htGrid1->GetOutputPort() );
  vtkNew<vtkHyperTreeGridGeometry> geometry2;
  geometry2->SetInputConnection( htGrid2->GetOutputPort() );

  // Shrink
  vtkNew<vtkShrinkFilter> shrink;
  shrink->SetInputConnection( geometry1->GetOutputPort() );
  shrink->SetShrinkFactor( .85 );

  // Mappers
  geometry1->Update();
  vtkPolyData* pd1 = geometry1->GetOutput();
  geometry2->Update();
  vtkPolyData* pd2 = geometry2->GetOutput();
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
  vtkMapper::SetResolveCoincidentTopologyPolygonOffsetParameters( 1, 1 );
  vtkNew<vtkDataSetMapper> mapper1;
  mapper1->SetInputConnection( shrink->GetOutputPort() );
  mapper1->SetScalarRange( pd1->GetCellData()->GetScalars()->GetRange() );
  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputConnection( geometry2->GetOutputPort() );
  mapper2->SetScalarRange( pd2->GetCellData()->GetScalars()->GetRange() );
 
  // Actors
  vtkNew<vtkActor> actor1;
  actor1->SetMapper( mapper1.GetPointer() );
  vtkNew<vtkActor> actor2;
  actor2->SetMapper( mapper2.GetPointer() );
  actor2->GetProperty()->SetRepresentationToWireframe();

  // Camera
  double bd1[6];
  pd1->GetBounds( bd1 );
  double bd2[6];
  pd2->GetBounds( bd2 );
  double bd[4];
  for ( int i = 0; i < 3; ++ i )
    {
    bd[i] = bd1[i] < bd2[i] ? bd1[i] : bd2[i];
    ++ i;
    bd[i] = bd1[i] > bd2[i] ? bd1[i] : bd2[i];
    }
  vtkNew<vtkCamera> camera;
  camera->SetClippingRange( 1., 100. );
  double xc = .5 * ( bd[0] + bd[1] );
  double yc = .5 * ( bd[2] + bd[3] );
  camera->SetFocalPoint( xc, yc, 0. );
  camera->SetPosition( xc, yc, 2. );

  // Renderer
  vtkNew<vtkRenderer> renderer;
  renderer->SetActiveCamera( camera.GetPointer() );
  renderer->SetBackground( 1., 1., 1. );
  renderer->AddActor( actor1.GetPointer() );
  renderer->AddActor( actor2.GetPointer() );

  // Render window
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer( renderer.GetPointer() );
  renWin->SetSize( 600, 200 );
  renWin->SetMultiSamples( 0 );

  // Interactor
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow( renWin.GetPointer() );

  // Render and test
  renWin->Render();
  
  int retVal = vtkRegressionTestImage( renWin.GetPointer() );
  if ( retVal == vtkRegressionTester::DO_INTERACTOR )
    {
    iren->Start();
    }

  return !retVal;
}
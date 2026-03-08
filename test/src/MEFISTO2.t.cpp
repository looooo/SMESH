#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <iostream>

#ifdef _WIN32
#include "crash_handler.h"
#endif

#define DBG(msg) do { std::cerr << "[MEFISTO2] " << msg << std::endl; } while(0)

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <SMESH_Gen.hxx>
#include <SMESH_Mesh.hxx>
#include <StdMeshers_LocalLength.hxx>
#include <StdMeshers_Regular_1D.hxx>
#include <StdMeshers_MEFISTO_2D.hxx>
#include <StdMeshers_QuadranglePreference.hxx>

TEST_CASE("Mesh the faces of a box.", "[MEFISTO2][Box]") {

	DBG("1: Create box");
	TopoDS_Solid box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Solid();

	DBG("2: Create SMESH_Gen");
	SMESH_Gen* gen = new SMESH_Gen();
	DBG("3: CreateMesh");
	SMESH_Mesh* mesh = gen->CreateMesh(true);

	DBG("4: Create 1D hypothesis and algo");
	StdMeshers_LocalLength* hyp1d = new StdMeshers_LocalLength(0, gen);
	hyp1d->SetLength(1.0);
	StdMeshers_Regular_1D* algo1d = new StdMeshers_Regular_1D(1, gen);

	DBG("5: Create 2D hypothesis and MEFISTO algo");
	StdMeshers_QuadranglePreference* hyp2d = new StdMeshers_QuadranglePreference(2, gen);
	StdMeshers_MEFISTO_2D* algo2d = new StdMeshers_MEFISTO_2D(3, gen);

	DBG("6: ShapeToMesh + AddHypothesis");
	mesh->ShapeToMesh(box);
	mesh->AddHypothesis(box, 0);
	mesh->AddHypothesis(box, 1);
	mesh->AddHypothesis(box, 2);
	mesh->AddHypothesis(box, 3);

	DBG("7: Compute (MEFISTO2 triangulation)");
	bool success = gen->Compute(*mesh, box);
	DBG("8: Compute done, success=" << success);
	REQUIRE(success == true);

	DBG("9: Check mesh counts");
	REQUIRE(mesh->NbNodes() == 1244);
	REQUIRE(mesh->NbTriangles() == 2484);
	REQUIRE(mesh->NbQuadrangles() == 0);

	DBG("10: Cleanup");
	delete hyp1d;
	delete algo1d;
	delete hyp2d;
	delete algo2d;
	delete mesh;
	delete gen;
	DBG("11: Done");
}

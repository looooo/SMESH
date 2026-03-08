#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <iostream>

#ifdef _WIN32
#include "crash_handler.h"
#endif

#define DBG(msg) do { std::cerr << "[StdMeshers] " << msg << std::endl; } while(0)

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <SMESH_Gen.hxx>
#include <SMESH_Mesh.hxx>
#include <SMESH_Hypothesis.hxx>
#include <StdMeshers_LocalLength.hxx>
#include <StdMeshers_Regular_1D.hxx>

TEST_CASE("Mesh an edge of a box.", "[StdMeshers][LocalLength]") {

	DBG("1: Create box");
	TopoDS_Solid box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Solid();

	DBG("2: Get edge");
	TopExp_Explorer exp = TopExp_Explorer(box, TopAbs_EDGE);
	const TopoDS_Shape& edge = exp.Current();

	DBG("3: Create SMESH_Gen + CreateMesh");
	SMESH_Gen* gen = new SMESH_Gen();
	SMESH_Mesh* mesh = gen->CreateMesh(true);

	DBG("4: Create hypothesis + algo");
	StdMeshers_LocalLength* hyp1d = new StdMeshers_LocalLength(0, gen);
	hyp1d->SetLength(0.1);
	StdMeshers_Regular_1D* algo1d = new StdMeshers_Regular_1D(1, gen);

	DBG("5: ShapeToMesh + AddHypothesis");
	mesh->ShapeToMesh(box);
	mesh->AddHypothesis(edge, 0);
	mesh->AddHypothesis(edge, 1);

	DBG("6: Compute");
	bool success = gen->Compute(*mesh, box);
	DBG("7: Compute done, success=" << success);
	REQUIRE(success == true);

	DBG("8: Check NbNodes");
	REQUIRE(mesh->NbNodes() == 107);

	DBG("9: Cleanup");
	delete hyp1d;
	delete algo1d;
	delete mesh;
	delete gen;
	DBG("10: Done");
}

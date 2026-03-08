#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <iostream>

#ifdef _WIN32
#include "crash_handler.h"
#endif

#define DBG(msg) do { std::cerr << "[NETGEN] " << msg << std::endl; } while(0)

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRepAlgoAPI_Splitter.hxx>
#include <TopTools_ListOfShape.hxx>

#include <SMESH_Gen.hxx>
#include <SMESH_Mesh.hxx>
#include <SMESH_Hypothesis.hxx>
#include <NETGENPlugin_SimpleHypothesis_2D.hxx>
#include <NETGENPlugin_SimpleHypothesis_3D.hxx>
#include <NETGENPlugin_NETGEN_2D.hxx>
#include <NETGENPlugin_NETGEN_2D3D.hxx>
#include <StdMeshers_LocalLength.hxx>
#include <StdMeshers_Regular_1D.hxx>

TEST_CASE("Mesh a box with tetrahedral elements.", "[NETGENPlugin]") {

    DBG("1: Create box");
    TopoDS_Solid box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Solid();

    DBG("2: Create SMESH_Gen + CreateMesh");
    SMESH_Gen* gen = new SMESH_Gen();
    SMESH_Mesh* mesh = gen->CreateMesh(true);

    DBG("3: Create hypothesis + algo");
    NETGENPlugin_SimpleHypothesis_3D* hyp = new NETGENPlugin_SimpleHypothesis_3D(0, gen);
    hyp->SetLocalLength(1.0);

    NETGENPlugin_NETGEN_2D3D* algo = new NETGENPlugin_NETGEN_2D3D(1, gen);

    DBG("4: ShapeToMesh + AddHypothesis");
    mesh->ShapeToMesh(box);
    mesh->AddHypothesis(box, 0);
    mesh->AddHypothesis(box, 1);

    DBG("5: Compute");
    bool success = gen->Compute(*mesh, box);
    DBG("6: Compute done, success=" << success);
    REQUIRE(success == true);

    DBG("7: Check mesh counts");
    REQUIRE(mesh->NbTetras() == 5087);
    REQUIRE(mesh->NbNodes() == 1243);

    DBG("8: Cleanup");
    delete hyp;
    delete algo;
    delete mesh;
    delete gen;
    DBG("9: Done");
}

TEST_CASE("Mesh a box with tetrahedral elements and a local edge length.", "[NETGENPlugin]") {

	DBG("1: Create box + get edge");
	TopoDS_Solid box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Solid();

	TopExp_Explorer exp = TopExp_Explorer(box, TopAbs_EDGE);
	const TopoDS_Shape& edge = exp.Current();

	DBG("2: Create SMESH_Gen + CreateMesh");
	SMESH_Gen* gen = new SMESH_Gen();
	SMESH_Mesh* mesh = gen->CreateMesh(true);

	DBG("3: Create hypotheses + algos");
	NETGENPlugin_SimpleHypothesis_3D* hyp3d = new NETGENPlugin_SimpleHypothesis_3D(0, gen);
	hyp3d->SetLocalLength(1.0);
	NETGENPlugin_NETGEN_2D3D* algo3d = new NETGENPlugin_NETGEN_2D3D(1, gen);

	StdMeshers_LocalLength* hyp1d = new StdMeshers_LocalLength(2, gen);
	hyp1d->SetLength(0.1);
	StdMeshers_Regular_1D* algo1d = new StdMeshers_Regular_1D(3, gen);

	DBG("4: ShapeToMesh + AddHypothesis");
	mesh->ShapeToMesh(box);
	mesh->AddHypothesis(box, 0);
	mesh->AddHypothesis(box, 1);
	mesh->AddHypothesis(edge, 2);
	mesh->AddHypothesis(edge, 3);

	DBG("5: Compute");
	bool success = gen->Compute(*mesh, box);
	DBG("6: Compute done, success=" << success);
	REQUIRE(success == true);

	DBG("7: Check mesh counts");
	REQUIRE(mesh->NbTetras() > 35000);
	REQUIRE(mesh->NbTetras() < 38000);
	REQUIRE(mesh->NbNodes() > 6500);
	REQUIRE(mesh->NbNodes() < 8000);

	DBG("8: Cleanup");
	delete hyp3d;
	delete algo3d;
	delete hyp1d;
	delete algo1d;
	delete mesh;
	delete gen;
	DBG("9: Done");
}

TEST_CASE("Mesh a box with tetrahedral elements and split faces.", "[NETGENPlugin]") {

	DBG("1: Create box");
	TopoDS_Solid box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Solid();

	DBG("2: Create split plane + Splitter");
	gp_Pnt origin = gp_Pnt(5.0, 0.0, 0.0);
	gp_Dir normal = gp_Dir(1.0, 0.0, 0.0);
	gp_Pln pln = gp_Pln(origin, normal);

	TopoDS_Face face = BRepBuilderAPI_MakeFace(pln, -5.0, 5.0, -5.0, 5.0).Face();

	TopTools_ListOfShape args = TopTools_ListOfShape();
	args.Append(box);

	TopTools_ListOfShape tools = TopTools_ListOfShape();
	tools.Append(face);

	BRepAlgoAPI_Splitter splitter = BRepAlgoAPI_Splitter();
	splitter.SetArguments(args);
	splitter.SetTools(tools);
	splitter.Build();
	REQUIRE(splitter.IsDone() == true);

	TopoDS_Shape split_box = splitter.Shape();

	DBG("3: Create SMESH_Gen + CreateMesh");
	SMESH_Gen* gen = new SMESH_Gen();
	SMESH_Mesh* mesh = gen->CreateMesh(true);

	DBG("4: Create hypothesis + algo");
	NETGENPlugin_SimpleHypothesis_3D* hyp = new NETGENPlugin_SimpleHypothesis_3D(0, gen);
	hyp->SetLocalLength(1.0);

	NETGENPlugin_NETGEN_2D3D* algo = new NETGENPlugin_NETGEN_2D3D(1, gen);

	DBG("5: ShapeToMesh + AddHypothesis");
	mesh->ShapeToMesh(split_box);
	mesh->AddHypothesis(split_box, 0);
	mesh->AddHypothesis(split_box, 1);

	DBG("6: Compute");
	bool success = gen->Compute(*mesh, split_box);
	DBG("7: Compute done");
	REQUIRE(success == true);

	DBG("8: Check mesh counts");
	REQUIRE(mesh->NbTetras() == 4905);
	REQUIRE(mesh->NbNodes() == 1215);

	DBG("9: Cleanup");
	delete hyp;
	delete algo;
	delete mesh;
	delete gen;
	DBG("10: Done");
}

TEST_CASE("Mesh a box with quadrilateral elements and split faces.", "[NETGENPlugin]") {

	DBG("1: Create box");
	TopoDS_Solid box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Solid();

	DBG("2: Create split plane");
	gp_Pnt origin = gp_Pnt(5.0, 0.0, 0.0);
	gp_Dir normal = gp_Dir(1.0, 0.0, 0.0);
	gp_Pln pln = gp_Pln(origin, normal);

	TopoDS_Face face = BRepBuilderAPI_MakeFace(pln, -5.0, 5.0, -5.0, 5.0).Face();

	TopTools_ListOfShape args = TopTools_ListOfShape();
	args.Append(box);

	TopTools_ListOfShape tools = TopTools_ListOfShape();
	tools.Append(face);

	BRepAlgoAPI_Splitter splitter = BRepAlgoAPI_Splitter();
	splitter.SetArguments(args);
	splitter.SetTools(tools);
	splitter.Build();
	REQUIRE(splitter.IsDone() == true);

	TopoDS_Shape split_box = splitter.Shape();

	DBG("3: Create SMESH_Gen + CreateMesh");
	SMESH_Gen* gen = new SMESH_Gen();
	SMESH_Mesh* mesh = gen->CreateMesh(true);

	DBG("4: Create 2D hypothesis + algo");
	NETGENPlugin_SimpleHypothesis_2D* hyp = new NETGENPlugin_SimpleHypothesis_2D(0, gen);
	hyp->SetLocalLength(1.0);
	hyp->SetAllowQuadrangles(true);

	NETGENPlugin_NETGEN_2D* algo = new NETGENPlugin_NETGEN_2D(1, gen);

	DBG("5: ShapeToMesh + AddHypothesis");
	mesh->ShapeToMesh(split_box);
	mesh->AddHypothesis(split_box, 0);
	mesh->AddHypothesis(split_box, 1);

	DBG("6: Compute");
	bool success = gen->Compute(*mesh, split_box);
	DBG("7: Compute done");
	REQUIRE(success == true);

	DBG("8: Check mesh counts");
	REQUIRE(mesh->NbFaces() == 625);
	REQUIRE(mesh->NbTriangles() == 0);
	REQUIRE(mesh->NbQuadrangles() == 625);
	REQUIRE(mesh->NbNodes() == 627);

	DBG("9: Cleanup");
	delete hyp;
	delete algo;
	delete mesh;
	delete gen;
	DBG("10: Done");
}
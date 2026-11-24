window.BENCHMARK_DATA = {
  "lastUpdate": 1795555529782,
  "repoUrl": "https://github.com/mircomarahrens/nconpp",
  "entries": {
    "NCONPP C++ Benchmarks": [
      {
        "commit": {
          "author": {
            "email": "mirco.marahrens@googlemail.com",
            "name": "Mirco Marahrens",
            "username": "mircomarahrens"
          },
          "committer": {
            "email": "mirco.marahrens@googlemail.com",
            "name": "Mirco Marahrens",
            "username": "mircomarahrens"
          },
          "distinct": true,
          "id": "38d6a03bcbe72f3d3f36b7b03d3cff947cc13da1",
          "message": "feat(benchmarks): add Google Benchmark suite, CI workflow, and poe task\n\n- Add C++ benchmark suite using Google Benchmark (`benchmarks/nconpp_benchmarks.cpp`,\n  `benchmarks/benchmarks.cmake`) covering Graph, LatticeGraph, LinAlg/Tensor\n  (`tensordot`, `outer`, `hosvd`), and TensorNetwork (`getLegs`, `contract`, `split`).\n- Wire `NCONPP_BUILD_BENCHMARKS` option (default ON) into `CMakeLists.txt` with\n  `-O3`/`/O2` optimization enabled for the benchmark target.\n- Add `.github/workflows/benchmarks.yml` to run benchmarks in Release mode,\n  publish job summaries/artifacts, and persist history on a `benchmarks-data`\n  orphan branch via `github-action-benchmark`.\n- Add `poe benchmark` task to `pyproject.toml` and document benchmark usage in `README.md`.",
          "timestamp": "2026-11-24T22:17:13+01:00",
          "tree_id": "ef2c2cd54aa94ce53a48c4844f65b529f23da586",
          "url": "https://github.com/mircomarahrens/nconpp/commit/38d6a03bcbe72f3d3f36b7b03d3cff947cc13da1"
        },
        "date": 1795555529250,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_Graph_MergeVerticesChain/200",
            "value": 925.2248068783745,
            "unit": "us/iter",
            "extra": "iterations: 756\ncpu: 925.1746230158732 us\nthreads: 1"
          },
          {
            "name": "BM_LatticeGraph_InitHoneycomb32x32PBC",
            "value": 659.9852857142888,
            "unit": "us/iter",
            "extra": "iterations: 1057\ncpu: 659.8817369914851 us\nthreads: 1"
          },
          {
            "name": "BM_LatticeGraph_RemoveEdgelessBoundary32x32OBC",
            "value": 189.65502115615146,
            "unit": "us/iter",
            "extra": "iterations: 3687\ncpu: 189.62386303227842 us\nthreads: 1"
          },
          {
            "name": "BM_Tensor_TensordotContiguous",
            "value": 168.62481556682252,
            "unit": "us/iter",
            "extra": "iterations: 4137\ncpu: 168.6066381435824 us\nthreads: 1"
          },
          {
            "name": "BM_Tensor_TensordotNonContiguous",
            "value": 198.31161739914398,
            "unit": "us/iter",
            "extra": "iterations: 3322\ncpu: 198.26279259482214 us\nthreads: 1"
          },
          {
            "name": "BM_Tensor_Outer32x32",
            "value": 1947.36558823526,
            "unit": "us/iter",
            "extra": "iterations: 357\ncpu: 1947.3099159663882 us\nthreads: 1"
          },
          {
            "name": "BM_LinAlg_HOSVD_12x12x12x12/0",
            "value": 2534.5287309090218,
            "unit": "us/iter",
            "extra": "iterations: 275\ncpu: 2534.362534545451 us\nthreads: 1"
          },
          {
            "name": "BM_LinAlg_HOSVD_12x12x12x12/6",
            "value": 2105.361084084045,
            "unit": "us/iter",
            "extra": "iterations: 333\ncpu: 2105.288198198194 us\nthreads: 1"
          },
          {
            "name": "BM_TensorNetwork_GetLegs",
            "value": 3.252841679130767,
            "unit": "us/iter",
            "extra": "iterations: 219971\ncpu: 3.252695778079844 us\nthreads: 1"
          },
          {
            "name": "BM_TensorNetwork_ContractMPS",
            "value": 71.987016835299,
            "unit": "us/iter",
            "extra": "iterations: 9801\ncpu: 71.99727078869502 us\nthreads: 1"
          },
          {
            "name": "BM_TensorNetwork_ContractLadder/0",
            "value": 346.01227173374457,
            "unit": "us/iter",
            "extra": "iterations: 2013\ncpu: 346.0099205166394 us\nthreads: 1"
          },
          {
            "name": "BM_TensorNetwork_ContractLadder/1",
            "value": 209.26233662072923,
            "unit": "us/iter",
            "extra": "iterations: 3345\ncpu: 209.25401464873562 us\nthreads: 1"
          },
          {
            "name": "BM_TensorNetwork_ContractWithSV",
            "value": 26.975139031649146,
            "unit": "us/iter",
            "extra": "iterations: 25843\ncpu: 26.976587083547678 us\nthreads: 1"
          },
          {
            "name": "BM_TensorNetwork_Split",
            "value": 581.3344808668865,
            "unit": "us/iter",
            "extra": "iterations: 1202\ncpu: 581.3624642262632 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}
#include <d3d11.h>
#include <xnamath.h>
#include "stdio.h"
#include "Core.h"
#include "GpuProfiler.h"
#include "Input.h"
#include "Text.h"

// We double buffer the queries
static ID3D11Query* pipeline_statistics[2];
static ID3D11Query* timestamp_disjoint_query[2];
static ID3D11Query* timestamp_begin_frame[2];
static ID3D11Query* timestamp_end_frame[2];

void GpuProfiler::Startup(ID3D11Device* device)
{
    for (int i=0; i<2; i++)
    {
        D3D11_QUERY_DESC querydesc;
        querydesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
        querydesc.MiscFlags = 0;
        device->CreateQuery(&querydesc, &pipeline_statistics[i]);

        querydesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
        querydesc.MiscFlags = 0;
        device->CreateQuery(&querydesc, &timestamp_disjoint_query[i]);

        querydesc.Query = D3D11_QUERY_TIMESTAMP;
        querydesc.MiscFlags = 0;
        device->CreateQuery(&querydesc, &timestamp_begin_frame[i]);
        device->CreateQuery(&querydesc, &timestamp_end_frame[i]);
    }
}

void GpuProfiler::Shutdown()
{
    for (int i=0; i<2; i++)
    {
        SAFE_RELEASE(pipeline_statistics[i]);
        SAFE_RELEASE(timestamp_disjoint_query[i]);
        SAFE_RELEASE(timestamp_begin_frame[i]);
        SAFE_RELEASE(timestamp_end_frame[i]);
    }
}

void GpuProfiler::BeginFrame(ID3D11DeviceContext* ctxt, uint framecount)
{
    // Pipeline statistics are gathered between the query begin/end
    ctxt->Begin(pipeline_statistics[framecount%2]);

    // All timestamp queries must be inside a timestamp disjoint begin/end bracket
    ctxt->Begin(timestamp_disjoint_query[framecount%2]);

    // To issue a timestamp, only the End() call needs to be made (Begin() is ignored)
    ctxt->End(timestamp_begin_frame[framecount%2]);
}

void GpuProfiler::EndFrame(ID3D11DeviceContext* ctxt, uint framecount)
{
    ctxt->End(timestamp_end_frame[framecount%2]);

    ctxt->End(timestamp_disjoint_query[framecount%2]);

    ctxt->End(pipeline_statistics[framecount%2]);

    // We double-buffer the timestamp queries. If this is the zeroth frame, then
    // we haven't flushed any queries to the command buffer yet.
    if (framecount > 0)
    {
        // Determine which frame has completed queries
        int profiling_frame = (framecount-1) % 2;

        // Get the timestamp disjoint, which tells us the frequency and if the timestamps are valid
        // We loop until the results are ready, which has the result of syncing the CPU with the previous
        // frame on the GPU
        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestampinfo;
        timestampinfo.Frequency = 0;
        while (timestampinfo.Frequency == 0)
        {
            ctxt->GetData(timestamp_disjoint_query[profiling_frame], &timestampinfo, sizeof(timestampinfo), 0);
        }

        // Once the timestamp disjoint is finished, then all the actual timestamps are also ready
        UINT64 begin = 0;
        UINT64 end = 0;
        ctxt->GetData(timestamp_begin_frame[profiling_frame], &begin, sizeof(begin), 0);
        ctxt->GetData(timestamp_end_frame[profiling_frame], &end, sizeof(end), 0);

        // Print results
        if (timestampinfo.Disjoint == TRUE)
        {
            Text::Print(0, 10, "Disjoint");
        } else {
            Text::Print(0, 10, "GPU: %3.1f ms", float((end - begin) / (double)timestampinfo.Frequency) * 1000.0f);
        }

        D3D11_QUERY_DATA_PIPELINE_STATISTICS stats;
        ctxt->GetData(pipeline_statistics[profiling_frame], &stats, sizeof(stats), 0);

        static uint show_pipeline_stats = 0;
        if (Input::key_down[VK_TAB])
            show_pipeline_stats ^= 1;

        if (show_pipeline_stats)
        {
            Text::Print(10, 20,  "IAVertices: %llu\n", stats.IAVertices);
            Text::Print(10, 30,  "IAPrimitives: %llu\n", stats.IAPrimitives);
            Text::Print(10, 40,  "VSInvocations: %llu\n", stats.VSInvocations);
            Text::Print(10, 50,  "GSInvocations: %llu\n", stats.GSInvocations);
            Text::Print(10, 60,  "GSPrimitives: %llu\n", stats.GSPrimitives);
            Text::Print(10, 70,  "CInvocations: %llu\n", stats.CInvocations);
            Text::Print(10, 80,  "CPrimitives: %llu\n", stats.CPrimitives);
            Text::Print(10, 90,  "PSInvocations: %llu\n", stats.PSInvocations);
            Text::Print(10, 100, "HSInvocations: %llu\n", stats.HSInvocations);
            Text::Print(10, 110, "DSInvocations: %llu\n", stats.DSInvocations);
            Text::Print(10, 120, "CSInvocations: %llu\n", stats.CSInvocations);
        }
    }
}

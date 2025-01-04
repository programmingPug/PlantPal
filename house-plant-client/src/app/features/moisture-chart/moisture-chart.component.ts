import { Component, OnInit, OnDestroy, ViewChild } from '@angular/core';
import { Subject } from 'rxjs';
import { takeUntil } from 'rxjs/operators';
import { SoilDataService } from '../../services/soil-data.service';
import { ChartConfiguration } from 'chart.js';

import { Chart, registerables } from 'chart.js';
import { BaseChartDirective } from 'ng2-charts';
Chart.register(...registerables);

@Component({
    selector: 'app-moisture-chart',
    templateUrl: './moisture-chart.component.html',
    styleUrls: ['./moisture-chart.component.scss'],
    standalone: false
})
export class MoistureChartComponent implements OnInit, OnDestroy {
  @ViewChild(BaseChartDirective) chart?: BaseChartDirective;
  private destroy$ = new Subject<void>();

  public lineChartData: ChartConfiguration['data'] = {
    datasets: [
      {
        data: [],
        label: 'Soil Moisture',
        backgroundColor: 'rgba(148,159,177,0.2)',
        borderColor: 'rgba(148,159,177,1)',
        pointBackgroundColor: 'rgba(148,159,177,1)',
        fill: true,
      }
    ],
    labels: []
  };

  public lineChartOptions: ChartConfiguration['options'] = {
    responsive: true,
    scales: {
      y: {
        beginAtZero: true
      }
    }
  };

  constructor(private soilDataService: SoilDataService) {}

  ngOnInit(): void {
    this.soilDataService.getSoilMoistureStream()
      .pipe(takeUntil(this.destroy$))
      .subscribe(data => {
        this.updateChart(data);
      });
  }

  ngOnDestroy(): void {
    this.destroy$.next();
    this.destroy$.complete();
  }

  private updateChart(data: any): void {
    const timestamp = new Date().toLocaleTimeString();
    
    if (this.lineChartData.datasets[0].data.length > 10) {
      this.lineChartData.datasets[0].data.shift();
      this.lineChartData.labels?.shift();
    }

    this.lineChartData.datasets[0].data.push(data.moisturePercentage);
    this.lineChartData.labels?.push(timestamp);
    this.chart?.update();
  }
}
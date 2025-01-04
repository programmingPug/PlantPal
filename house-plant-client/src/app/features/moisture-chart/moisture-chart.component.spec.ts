import { ComponentFixture, TestBed } from '@angular/core/testing';

import { MoistureChartComponent } from './moisture-chart.component';

describe('MoistureChartComponent', () => {
  let component: MoistureChartComponent;
  let fixture: ComponentFixture<MoistureChartComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ MoistureChartComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(MoistureChartComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});

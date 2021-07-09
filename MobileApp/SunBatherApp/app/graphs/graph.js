import { GraphViewModel } from './graph-view-model'
import { DatePicker } from '@nativescript/core'

export function onNavigatingTo(args) {
  const component = args.object;
  component.bindingContext = new GraphViewModel();
}
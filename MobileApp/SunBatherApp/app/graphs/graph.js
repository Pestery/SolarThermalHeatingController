import { GraphViewModel } from './graph-view-model'

export function onNavigatingTo(args) {
  const component = args.object;
  component.bindingContext = new GraphViewModel();
}